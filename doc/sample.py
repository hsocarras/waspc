import sys

# --- Clases para representar las estructuras de WebAssembly ---

class Store:
    """
    El Almacén (Store) contiene todos los objetos globales en tiempo de ejecución.
    En una implementación real, manejaría la asignación y el ciclo de vida de todos
    los objetos Wasm.
    """
    def __init__(self):
        self.funcs = []
        self.tables = []
        self.mems = []
        self.globals = []

class Module:
    """
    Representa un módulo Wasm decodificado y validado.
    Esta es la entrada estática para el proceso de instanciación.
    """
    def __init__(self):
        self.types = []       # Lista de tipos de función (firmas)
        self.imports = []     # Lista de descriptores de importación
        self.functions = []   # Índices de tipo para las funciones locales
        self.tables = []      # Descriptores de tablas locales
        self.memories = []    # Descriptores de memorias locales
        self.globals = []     # Descriptores de globales locales
        self.exports = []     # Lista de descriptores de exportación
        self.start = None     # Índice de la función de inicio (opcional)
        self.code = []        # Cuerpos de las funciones locales
        self.elements = []    # Segmentos de elementos para inicializar tablas
        self.data = []        # Segmentos de datos para inicializar memorias

class ModuleInstance:
    """
    Representa una instancia activa y con estado de un módulo.
    Contiene las "direcciones" de los objetos que le pertenecen en el Store.
    """
    def __init__(self):
        self.types = []
        self.func_addrs = []
        self.table_addrs = []
        self.mem_addrs = []
        self.global_addrs = []
        self.exports = {} # Diccionario de {nombre_export: objeto_exportado}

# --- Clases para representar los objetos en tiempo de ejecución ---

class FunctionInstance:
    """Representa una función ejecutable."""
    def __init__(self, func_type, module_instance, code):
        self.type = func_type
        self.module = module_instance # La instancia a la que pertenece
        self.code = code # El código de la función
    
    def __call__(self, *args):
        # En una implementación real, aquí estaría el intérprete o JIT
        # que ejecuta el bytecode de la función.
        print(f"Ejecutando función con firma {self.type} y argumentos: {args}")
        # Placeholder: Simplemente devolvemos un valor por defecto.
        return 0

class TableInstance:
    """Representa una tabla de referencias."""
    def __init__(self, table_type):
        self.type = table_type
        self.elements = [None] * table_type['limits']['min']
        # Una implementación completa manejaría el crecimiento hasta 'max'.

class MemoryInstance:
    """Representa una memoria lineal."""
    def __init__(self, mem_type):
        self.type = mem_type
        # La memoria se asigna en páginas de 64KiB.
        self.buffer = bytearray(mem_type['limits']['min'] * (64 * 1024))

class GlobalInstance:
    """Representa una variable global."""
    def __init__(self, global_type, value):
        self.type = global_type
        self.value = value
        self.mutable = global_type['mutable']

class StructInstance:
    """Representa una instancia de una estructura (Wasm GC - Tipo Recursivo)."""
    def __init__(self, type_index, fields):
        self.type_index = type_index
        self.fields = fields # Lista de valores de los campos
    
    def __repr__(self):
        return f"<StructInstance type={self.type_index} fields={self.fields}>"

class ArrayInstance:
    """Representa una instancia de un array (Wasm GC - Tipo Recursivo)."""
    def __init__(self, type_index, elements):
        self.type_index = type_index
        self.elements = elements # Lista de elementos
    
    def __repr__(self):
        return f"<ArrayInstance type={self.type_index} len={len(self.elements)}>"

# --- Funciones auxiliares ---

def evaluate_const_expr(expr, instance, store):
    """
    Evalúa una expresión constante. Las expresiones de inicialización para
    globales, offsets de datos y elementos deben ser constantes.
    """
    # Placeholder: Una implementación real interpretaría un subconjunto
    # de instrucciones como i32.const, get_global, etc.
    # Asumimos que la expresión ya es el valor.
    if expr['opcode'] == 'i32.const':
        return expr['value']
    if expr['opcode'] == 'global.get':
        # Solo se permite acceder a globales importadas que son inmutables.
        addr = instance.global_addrs[expr['index']]
        glob = store.globals[addr]
        if glob.mutable:
            raise ValueError("La expresión constante no puede leer una global mutable.")
        return glob.value
    raise ValueError(f"Operación no soportada en expresión constante: {expr['opcode']}")

def roll(module_instance: ModuleInstance, type_index: int, values: list) -> object:
    """
    Implementa la operación 'roll' (plegar) de tipos iso-recursivos (Wasm 3.0 GC).
    
    En la teoría de tipos iso-recursivos, 'roll' toma una estructura concreta y la
    empaqueta en un tipo abstracto recursivo. En Wasm GC, esto equivale a las 
    instrucciones de asignación como `struct.new` o `array.new`.
    
    Args:
        module_instance: La instancia del módulo (para acceder a las definiciones de tipos).
        type_index: El índice del tipo recursivo (struct o array) en el módulo.
        values: La lista de valores para inicializar los campos o elementos.
    """
    if type_index >= len(module_instance.types):
        raise ValueError(f"Índice de tipo inválido: {type_index}")
    
    type_def = module_instance.types[type_index]
    
    # Verificamos si es un tipo GC (struct o array)
    kind = type_def.get('kind')
    
    if kind == 'struct':
        # Validación básica de aridad
        if len(values) != len(type_def['fields']):
            raise ValueError(f"Struct espera {len(type_def['fields'])} campos, recibidos {len(values)}")
        return StructInstance(type_index, values)
    
    elif kind == 'array':
        return ArrayInstance(type_index, values)
    
    elif 'params' in type_def:
        raise ValueError("No se puede hacer 'roll' sobre un tipo de función.")
    else:
        raise ValueError(f"Tipo desconocido o no soportado para roll: {type_def}")

def unroll(reference: object) -> list:
    """
    Implementa la operación 'unroll' (desplegar) de tipos iso-recursivos.
    
    Expone la representación subyacente (campos o elementos) de una referencia opaca.
    En Wasm, esto ocurre implícitamente al acceder a los datos (struct.get, array.get).
    """
    if isinstance(reference, StructInstance):
        return reference.fields
    elif isinstance(reference, ArrayInstance):
        return reference.elements
    else:
        raise ValueError(f"El objeto {reference} no es un tipo recursivo desplegable.")

# --- La función de Instanciación ---

def instantiate(module: Module, import_object: dict = None) -> ModuleInstance:
    """
    Algoritmo de instanciación de un módulo WebAssembly.
    
    Args:
        module: El módulo Wasm decodificado y validado.
        import_object: Un diccionario para resolver las importaciones del módulo.
                       Ej: {'env': {'my_func': lambda x: print(x)}}
    
    Returns:
        Una nueva ModuleInstance.
    """
    if import_object is None:
        import_object = {}

    # 1. Asignación (Allocation)
    store = Store()
    instance = ModuleInstance()
    instance.types = module.types

    # 2. Resolución de Importaciones
    for imp in module.imports:
        mod_name, name, desc = imp['module'], imp['name'], imp['desc']
        
        if mod_name not in import_object or name not in import_object[mod_name]:
            raise ImportError(f"Importación no resuelta: '{mod_name}.{name}'")
            
        external_val = import_object[mod_name][name]
        
        # Aquí iría la validación de tipos entre la importación y el valor externo.
        if desc['kind'] == 'function':
            # Asumimos que el valor externo es una función compatible.
            store.funcs.append(external_val)
            instance.func_addrs.append(len(store.funcs) - 1)
        # ... Lógica similar para 'table', 'memory', 'global'
    
    num_imports_func = len(instance.func_addrs)

    # 3. Instanciación de Definiciones Locales
    # Funciones
    for i, func_body in enumerate(module.code):
        func_type_index = module.functions[i]
        func_type = module.types[func_type_index]
        func_inst = FunctionInstance(func_type, instance, func_body)
        store.funcs.append(func_inst)
        instance.func_addrs.append(len(store.funcs) - 1)

    # Tablas
    for table_type in module.tables:
        table_inst = TableInstance(table_type)
        store.tables.append(table_inst)
        instance.table_addrs.append(len(store.tables) - 1)

    # Memorias
    for mem_type in module.memories:
        mem_inst = MemoryInstance(mem_type)
        store.mems.append(mem_inst)
        instance.mem_addrs.append(len(store.mems) - 1)

    # Globales
    for global_def in module.globals:
        init_value = evaluate_const_expr(global_def['init'], instance, store)
        global_inst = GlobalInstance(global_def['type'], init_value)
        store.globals.append(global_inst)
        instance.global_addrs.append(len(store.globals) - 1)

    # 4. Inicialización de Tablas y Memorias (procesando segmentos activos)
    # Segmentos de Elementos
    for elem_segment in module.elements:
        if elem_segment['mode'] == 'active':
            offset = evaluate_const_expr(elem_segment['offset'], instance, store)
            table_addr = instance.table_addrs[elem_segment['table_index']]
            table = store.tables[table_addr]
            
            for i, func_idx in enumerate(elem_segment['init']):
                if offset + i < len(table.elements):
                    func_addr = instance.func_addrs[func_idx]
                    table.elements[offset + i] = store.funcs[func_addr]

    # Segmentos de Datos
    for data_segment in module.data:
        if data_segment['mode'] == 'active':
            offset = evaluate_const_expr(data_segment['offset'], instance, store)
            mem_addr = instance.mem_addrs[data_segment['mem_index']]
            memory = store.mems[mem_addr]
            
            data_bytes = data_segment['init']
            if offset + len(data_bytes) <= len(memory.buffer):
                memory.buffer[offset : offset + len(data_bytes)] = data_bytes

    # 5. Creación de Exportaciones
    for export_def in module.exports:
        name = export_def['name']
        kind = export_def['kind']
        index = export_def['index']

        if kind == 'function':
            addr = instance.func_addrs[index]
            instance.exports[name] = store.funcs[addr]
        elif kind == 'table':
            addr = instance.table_addrs[index]
            instance.exports[name] = store.tables[addr]
        elif kind == 'memory':
            addr = instance.mem_addrs[index]
            instance.exports[name] = store.mems[addr]
        elif kind == 'global':
            addr = instance.global_addrs[index]
            instance.exports[name] = store.globals[addr]

    # 6. Ejecución de la Función de Inicio
    if module.start is not None:
        start_func_addr = instance.func_addrs[module.start]
        start_function = store.funcs[start_func_addr]
        start_function() # Se llama sin argumentos

    # 7. Retorno de la Instancia
    print("Instanciación completada exitosamente.")
    return instance

# --- Ejemplo de Uso ---
if __name__ == "__main__":
    # 1. Crear un módulo de ejemplo (simulando un .wat o .wasm ya parseado)
    # (module
    #   (import "js" "imported_func" (func $i (param i32)))
    #   (func $add (param $a i32) (param $b i32) (result i32)
    #     local.get $a
    #     local.get $b
    #     i32.add
    #   )
    #   (memory 1)
    #   (export "add" (func $add))
    #   (export "memory" (memory 0))
    # )
    
    print("Definiendo un módulo de ejemplo...")
    my_module = Module()

    # Tipos de función: (i32) -> (), (i32, i32) -> (i32)
    my_module.types = [
        {'params': ['i32'], 'results': []},
        {'params': ['i32', 'i32'], 'results': ['i32']}
    ]

    # Importaciones
    my_module.imports.append({
        'module': 'js', 'name': 'imported_func', 'desc': {'kind': 'function', 'type_index': 0}
    })

    # Funciones locales (solo una, $add)
    my_module.functions.append(1) # $add usa el tipo de función en el índice 1
    my_module.code.append({'locals': [], 'body': "bytecode_for_add"})

    # Memorias
    my_module.memories.append({'limits': {'min': 1, 'max': 1}})

    # Exportaciones
    my_module.exports.append({'name': 'add', 'kind': 'function', 'index': 1}) # index 0 es importada, 1 es local
    my_module.exports.append({'name': 'memory', 'kind': 'memory', 'index': 0})

    # Definición de un Tipo Recursivo (Lista Enlazada) para probar roll/unroll
    # (rec (type $Node (struct (field i32) (field (ref null $Node)))))
    # Índice de tipo: 2
    my_module.types.append({
        'kind': 'struct',
        'fields': [{'type': 'i32'}, {'type': 'ref', 'heap_type': 2, 'nullable': True}]
    })

    # 2. Crear un objeto de importación para satisfacer las dependencias
    def my_imported_func(x):
        print(f"[HOST] La función importada fue llamada con: {x}")

    my_import_object = {
        'js': {
            'imported_func': my_imported_func
        }
    }

    # 3. Instanciar el módulo
    print("\nIniciando instanciación...")
    try:
        instance = instantiate(my_module, my_import_object)
        
        # 4. Usar las exportaciones
        print("\nUsando las exportaciones de la instancia:")
        
        # Llamar a la función exportada 'add'
        add_func = instance.exports['add']
        result = add_func(10, 32) # Esto llamará a nuestro placeholder
        print(f"Resultado de add(10, 32) (simulado): {result}")

        # Acceder a la memoria exportada
        mem = instance.exports['memory']
        print(f"Tamaño de la memoria exportada: {len(mem.buffer)} bytes")
        
        # Escribir y leer de la memoria
        mem.buffer[0] = 65 # 'A'
        mem.buffer[1] = 66 # 'B'
        print(f"Leído de la memoria en la posición 0: {chr(mem.buffer[0])}")

        # 5. Probar roll y unroll con tipos recursivos
        print("\nProbando operaciones de Tipos Recursivos (GC):")
        
        # Crear el final de la lista: Node(20, None) -> roll
        node_end = roll(instance, 2, [20, None])
        print(f"Nodo final creado (roll): {node_end}")
        
        # Crear el inicio de la lista: Node(10, node_end) -> roll
        node_head = roll(instance, 2, [10, node_end])
        print(f"Nodo cabecera creado (roll): {node_head}")
        
        # Acceder a los datos -> unroll
        head_data = unroll(node_head)
        print(f"Datos del nodo cabecera (unroll): Valor={head_data[0]}, Siguiente={head_data[1]}")

    except (ImportError, ValueError) as e:
        print(f"Error durante la instanciación: {e}", file=sys.stderr)
