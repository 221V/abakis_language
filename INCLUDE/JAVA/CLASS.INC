; Macros to write .CLASS files -
; define attributes, fields, methods,
; etc - by Tomasz Grysztar...

macro u1 [v] { db v }
macro u2 [v] { db (v) shr 8,(v) and 0FFh }
macro u4 [v] { db (v) shr 24,((v) shr 16) \
 and 0FFh,((v) shr 8) and 0FFh,(v) and 0FFh }

macro constant_pool {

  u2 constant_pool_count
  constant_pool_counter = 1

  struc constant_utf8 [string] \{
    common
      . = constant_pool_counter
      constant_pool_counter = constant_pool_counter + 1
      local ..data,..length
      u1 1
      u2 ..length
      ..data: db string
      ..length = $ - ..data
  \}

  struc constant_integer value \{
    . = constant_pool_counter
    constant_pool_counter = constant_pool_counter + 1
    u1 3
    u4 value
  \}

  struc constant_float value \{
    . = constant_pool_counter
    constant_pool_counter = constant_pool_counter + 1
    u1 4
    u4 value
  \}

  struc constant_long value \{
    . = constant_pool_counter
    constant_pool_counter = constant_pool_counter + 1
    u1 5
    u4 value shr 32,value and 0FFFFFFFFh
  \}

  struc constant_double value \{
    . = constant_pool_counter
    constant_pool_counter = constant_pool_counter + 1
    u1 6
    u4 value shr 32,value and 0FFFFFFFFh
  \}

  struc constant_class name_index \{
    . = constant_pool_counter
    constant_pool_counter = constant_pool_counter + 1
    u1 7
    u2 name_index
  \}

  struc constant_string string_index \{
    . = constant_pool_counter
    constant_pool_counter = constant_pool_counter + 1
    u1 8
    u2 string_index
  \}

  struc constant_fieldref class_index,name_and_type_index \{
    . = constant_pool_counter
    constant_pool_counter = constant_pool_counter + 1
    u1 9
    u2 class_index
    u2 name_and_type_index
  \}

  struc constant_methodref class_index,name_and_type_index \{
    . = constant_pool_counter
    constant_pool_counter = constant_pool_counter + 1
    u1 10
    u2 class_index
    u2 name_and_type_index
  \}

  struc constant_interfacemethodref class_index,name_and_type_index \{
    . = constant_pool_counter
    constant_pool_counter = constant_pool_counter + 1
    u1 11
    u2 class_index
    u2 name_and_type_index
  \}

  struc constant_nameandtype name_index,descriptor_index \{
    . = constant_pool_counter
    constant_pool_counter = constant_pool_counter + 1
    u1 12
    u2 name_index
    u2 descriptor_index
  \}

}

macro end_constant_pool {
  constant_pool_count = constant_pool_counter
  restruc constant_utf8,constant_integer,constant_float,\
   constant_long,constant_double
  restruc constant_class,constant_string
  restruc constant_fieldref,constant_methodref,\
  constant_interfacemethodref,constant_nameandtype
}     

ACC_PUBLIC       = 0x0001
ACC_PRIVATE      = 0x0002
ACC_PROTECTED    = 0x0004
ACC_STATIC       = 0x0008
ACC_FINAL        = 0x0010
ACC_SUPER        = 0x0020
ACC_SYNCHRONIZED = 0x0020
ACC_NATIVE       = 0x0200
ACC_INTERFACE    = 0x0200
ACC_ABSTRACT     = 0x0400
ACC_STRICT       = 0x0800

;;;;;;;;;;;;;;;;;;; ATTRIBUTES ;;;;;;;;;;;;;;;;;;;

macro attributes {
  local count,counter
  attributes_count equ count
  attributes_counter equ counter
  u2 attributes_count
  attributes_counter = 0
  macro attribute attribute_name_index \{
    attributes_counter = attributes_counter + 1
    u2 attribute_name_index
    local start,length
    attribute_start equ start
    attribute_length equ length
    u4 attribute_length
    attribute_start = $
  \}
  macro end_attribute \{
    attribute_length = $ - attribute_start
    restore atribute_start,attribute_length
  \}
}

macro end_attributes {
  attributes_count = attributes_counter
  restore attributes_count,attributes_counter
  purge attribute
}

;;;;;;;;;;;;;;;;;;;;; FIELDS ;;;;;;;;;;;;;;;;;;;;;

macro fields {
  u2 fields_count
  fields_counter = 0
  macro field_info access_flags,\
   name_index,descriptor_index \{
    fields_counter = fields_counter + 1
    u2 access_flags
    u2 name_index
    u2 descriptor_index
    attributes
  \}
  macro end_field_info \{ end_attributes \}
}

macro end_fields {
  fields_count = fields_counter
  purge field_info,end_field_info
}

;;;;;;;;;;;;;;;;;;;;; METHODS ;;;;;;;;;;;;;;;;;;;;

macro methods {
  u2 methods_count
  methods_counter = 0
  macro method_info access_flags,\
   name_index,descriptor_index \{
    methods_counter = methods_counter + 1
    u2 access_flags
    u2 name_index
    u2 descriptor_index
    attributes
  \}
  macro end_method_info \{ end_attributes \}
}

macro end_methods {
  methods_count = methods_counter
  purge method_info,end_method_info
}

;;;;;;;;;;;;;;;;;;; INTERFACES ;;;;;;;;;;;;;;;;;;;

macro interfaces {
  u2 interfaces_count
  interfaces_counter = 0
  macro interface interface \{
    interfaces_counter = interfaces_counter + 1
    u2 interface
  \}
}

macro end_interfaces {
  interfaces_count = interfaces_counter
  purge interface
}

;;;;;;;;;;;;;;;;;;; BYTE-CODE ;;;;;;;;;;;;;;;;;;;;

macro bytecode {
  local length
  bytecode_length equ length
  u4 bytecode_length
  bytecode_offset = $
  org 0
}

macro end_bytecode {
  bytecode_length = $
  org bytecode_offset+bytecode_length
  restore bytecode_length
}

;;;;;;;;;;;;;;;;;;; EXCEPTIONS ;;;;;;;;;;;;;;;;;;;

macro exceptions {
  local length
  exception_table_length equ length
  u2 exception_table_length
  exception_counter = 0
  macro exception start_pc,end_pc,\
   handler_pc,catch_type \{
    exception_counter = exception_counter + 1
    u2 start_pc
    u2 end_pc
    u2 handler_pc
    u2 catch_type
  \}
}

macro end_exceptions {
  exception_table_length = exception_counter
  restore exception_table_length
}