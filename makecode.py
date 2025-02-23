import sys
import pyperclip
from colorama import Fore, Style


def main():
    global lastAFunc, lastWFunc
    global lastAStruct, lastWStruct
    for line in sys.stdin:
        tokenize(line)
        if lastAFunc is not None and lastWFunc is not None:
            generateFuncHook()
            lastAFunc = None
            lastWFunc = None
        if lastAStruct is not None and lastWStruct is not None:
            generateStructConvert()
            lastAStruct = None
            lastWStruct = None

    print(f'{Fore.RED}EOF{Style.RESET_ALL}')


def tokenize(code: str):
    token = ""
    for line in code:
        for char in line:
            if char.isalnum() or char == "_":
                token += char
            else:
                if token:
                    onNewToken(token)
                    token = ""
                if char.isspace():
                    pass
                else:
                    onNewToken(char)
    if token:
        onNewToken(token)


tokens = []
leftBracketCount = 0
LEFT_BRACKET = ["{", "(", "[", "<"]
RIGHT_BRACKET = ["}", ")", "]", ">"]


def onNewToken(token: str):
    global tokens, leftBracketCount
    if token in LEFT_BRACKET:
        leftBracketCount += 1
    elif token in RIGHT_BRACKET:
        leftBracketCount -= 1
    tokens.append(token)
    if leftBracketCount == 0 and token == ";":
        parse(tokens)
        tokens.clear()


def parse(tokens: list[str]):
    if tokens[0] == "typedef":
        parseStruct(tokens)
    else:
        parseFunction(tokens)


class Field:
    name: str
    type: str

    def __init__(self):
        self.name = None
        self.type = None


class Struct:
    name: str
    fields: list[Field]

    def __init__(self):
        self.name = None
        self.fields = []

    def addField(self, field: Field):
        self.fields.append(field)


lastAStruct: Struct = None
lastWStruct: Struct = None


def parseStruct(tokens: list[str]):
    global lastAStruct, lastWStruct
    struct = Struct()
    i = 3
    while i < len(tokens):
        if tokens[i] == '{':
            i += 1
            field = Field()
            while tokens[i] != '}':
                if field.type is None:
                    field.type = tokens[i]
                elif field.name is None:
                    field.name = tokens[i]
                elif tokens[i] == ';':
                    struct.addField(field)
                    field = Field()
                i += 1
        elif struct.name is None:
            struct.name = tokens[i]
            if struct.name[-1] == "A":
                lastAStruct = struct
            elif struct.name[-1] == "W":
                lastWStruct = struct
            else:
                raise Exception("not a aw struct")
        i += 1

    print(f'{Fore.GREEN}parsed struct {Fore.CYAN}{struct.name}{Style.RESET_ALL}')


class Argument:
    name: str
    type: str
    attr: list[str]
    const: bool
    printerDepth: int

    def __init__(self):
        self.name = None
        self.type = None
        self.attr = []
        self.const = False
        self.printerDepth = 0

    def addAttr(self, attr: str):
        self.attr.append(attr)

    def isIn(self):
        return "in" in self.attr

    def isOut(self):
        return "out" in self.attr

    def isOptional(self):
        return "optional" in self.attr

    def typeStr(self) -> str:
        s = ''
        if self.const:
            s += 'const '
        s += self.type
        s += '*' * self.printerDepth
        return s

    def __str__(self):
        s = ""
        s += '['
        s += ", ".join(self.attr)
        s += ']'
        if self.const:
            s += ' const'
        s += f' {self.type}'
        s += '*' * self.printerDepth
        s += f' {self.name}'
        return s


class Function:
    name: str
    returnType: str
    arguments: list[Argument]

    def __init__(self):
        self.name = None
        self.returnType = None
        self.arguments = []

    def addArgument(self, arg: Argument):
        self.arguments.append(arg)

    def __str__(self):
        s = ""
        s += f'{self.returnType} {self.name}(\n'
        for arg in self.arguments:
            s += f'{arg},\n'
        s += ');\n'
        return s


lastAFunc: Function = None
lastWFunc: Function = None


def parseFunction(tokens: list[str]):
    global lastAFunc, lastWFunc
    func = Function()
    i = 0
    while i < len(tokens):
        if func.returnType is None:
            func.returnType = tokens[i]
        elif func.name is None:
            func.name = tokens[i]
            if func.name[-1] == "A":
                lastAFunc = func
            elif func.name[-1] == "W":
                lastWFunc = func
            else:
                raise Exception("not a aw function")
        elif tokens[i] == '(':
            i += 1
            arg = Argument()
            func.addArgument(arg)
            while tokens[i] != ')':
                if tokens[i] == '[':
                    i += 1
                    while tokens[i] != ']':
                        if tokens[i].isalpha():
                            arg.addAttr(tokens[i])
                        i += 1
                elif tokens[i] == ',':
                    arg = Argument()
                    func.addArgument(arg)
                elif tokens[i] == 'const':
                    arg.const = True
                elif tokens[i] == '*':
                    arg.printerDepth += 1
                elif arg.type is None:
                    arg.type = tokens[i]
                elif arg.name is None:
                    arg.name = tokens[i]
                i += 1
        i += 1

    print(f'{Fore.GREEN}parsed function {Fore.CYAN}{func.name}{Style.RESET_ALL}')


def generateFuncHook():
    global lastAFunc, lastWFunc

    if lastAFunc.returnType != lastWFunc.returnType:
        raise Exception("return type not match")
    if len(lastAFunc.arguments) != len(lastWFunc.arguments):
        raise Exception("arguments count not match")

    funcHook = ""
    funcHook += f'FUNC_HOOK({lastAFunc.name}, {lastAFunc.returnType}, ('
    funcHook += ", ".join([f'{arg.typeStr()} {arg.name}' for arg in lastAFunc.arguments])
    funcHook += '), {\n'
    for i in range(len(lastAFunc.arguments)):
        if lastAFunc.arguments[i].type == lastWFunc.arguments[i].type:
            funcHook += f'    auto p{i} = {lastAFunc.arguments[i].name};\n'
        else:
            if lastAFunc.arguments[i].isOut():
                raise Exception("out argument not support yet")
            funcHook += f'    auto p{i} = {lastAFunc.arguments[i].type}to{lastWFunc.arguments[i].type}({lastAFunc.arguments[i].name});\n'
    funcHook += f'    auto ret = {lastWFunc.name}('
    funcHook += ", ".join([f'p{i}' for i in range(len(lastAFunc.arguments))])
    funcHook += ');\n'
    funcHook += '    return ret;\n'
    funcHook += '});\n'

    initHook = ""
    initHook += f'initHook_{lastAFunc.name}();\n'

    print(f'{Fore.YELLOW}copied FUNC_HOOK...{Style.RESET_ALL}')
    print(funcHook)
    print(initHook)

    pyperclip.copy(initHook)
    pyperclip.copy(funcHook)


def generateStructConvert():
    global lastAStruct, lastWStruct

    if len(lastAStruct.fields) != len(lastWStruct.fields):
        raise Exception("fields count not match")

    convert = ""
    convert += f'const {lastWStruct.name}* {lastAStruct.name}to{lastWStruct.name}(const {lastAStruct.name}* in) {{\n'
    convert += f'    auto out = new {lastWStruct.name};\n'
    for i in range(len(lastAStruct.fields)):
        if lastAStruct.fields[i].type == lastWStruct.fields[i].type:
            convert += f'    out->{lastWStruct.fields[i].name} = in->{lastAStruct.fields[i].name};\n'
        else:
            convert += f'    out->{lastWStruct.fields[i].name} = {lastAStruct.fields[i].type}to{lastWStruct.fields[i].type}(in->{lastAStruct.fields[i].name});\n'
    convert += '    return out;\n'
    convert += '}\n'

    print(f'{Fore.YELLOW}copied struct convert...{Style.RESET_ALL}')
    print(convert)

    pyperclip.copy(convert)


if __name__ == '__main__':
    main()
