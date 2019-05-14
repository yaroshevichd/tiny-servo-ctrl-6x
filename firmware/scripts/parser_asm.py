import parsy

@parsy.generate('loc')
def line_of_code():
    yield whitespace
    address = yield lexeme(number)
    yield colon
    byte_codes = yield lexeme((byte_code << parsy.string(' ')).times(1, max=4))
    mnemonic = yield lexeme(parsy.regex(r'\w{2,5}'))
    op1 = yield lexeme(operand.optional())
    yield lexeme(comma.optional())
    op2 = yield lexeme(operand.optional())
    yield lexeme(semicolon.optional())
    comment = yield lexeme(parsy.regex(r'.+').optional())
    return 'loc', address, byte_codes, mnemonic, op1, op2

@parsy.generate('fn')
def function():
    address = yield lexeme(number)
    yield parsy.string('<')
    fn_name = yield parsy.regex(r'[_\w\d\.]+')
    yield lexeme(parsy.string('>'))
    yield lexeme(parsy.string(':'))
    return 'fn', address, fn_name

@parsy.generate('junk')
def junk():
    return 'junk', (yield parsy.any_char.many().concat())

whitespace  = parsy.regex(r'\s*')
lexeme      = lambda p: p << whitespace
colon       = lexeme(parsy.string(':'))
semicolon   = lexeme(parsy.string(';'))
comma       = lexeme(parsy.string(','))
reg_word    = parsy.regex(r'(-?X\+?|-?(Y|Z)\+?\d*)')
reg_byte    = parsy.regex(r'r\d+')
number      = parsy.regex(r'-?(0x)?[\da-fA-F]+')
offset      = parsy.regex(r'\.(\+|-)\d+')
operand     = reg_word | reg_byte | number | offset
byte_code   = parsy.regex(r'[0-9a-f]{2}')
parser      = line_of_code | function | junk
