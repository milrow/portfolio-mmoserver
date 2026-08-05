import csv
from collections import defaultdict

messages = defaultdict(list)
enums = []

def generate_proto_from_csv(enum_csv_file_path, message_csv_file_path, output_proto_path):

    try:
        with open(enum_csv_file_path, mode='r', encoding='utf-8-sig') as f:
            reader = csv.DictReader(f)
            for row in reader:
                direction = row.get('Direction', '').strip()
                field_name = row.get('FieldName', '').strip()
                field_number = row.get('FieldNumber').strip()

                if not field_name:
                    continue
                
                if(direction.upper() == 'NONE'):
                    final_enum_name = field_name
                else:
                    final_enum_name = f"ID_{direction}_{field_name}"

                enums.append({
                'name' : final_enum_name,
                'id' : field_number
                })
            
            
    except FileNotFoundError:
        print(f"{enum_csv_file_path} 파일을 찾을 수 없습니다.")


    try:
        with open(message_csv_file_path, mode='r', encoding='utf-8-sig') as f:
            reader = csv.DictReader(f)
            for row in reader:
                msg_name = row.get('MessageName', '').strip()

                if not msg_name:
                    continue

                direction = row.get('Direction', '').strip()
                field_name = row.get('FieldName', '').strip()
                full_msg_name = f"{direction}_{msg_name}" if direction else msg_name

                if full_msg_name not in messages:
                    messages[full_msg_name] = []

                
                if field_name:
                    messages[full_msg_name].append({
                        'rule': row.get('Rule', '').strip(),
                        'type': row.get('FieldType', '').strip(),
                        'name': field_name,
                        'number': row.get('FieldNumber', '').strip()
                    })


    except FileNotFoundError:
        print(f"{message_csv_file_path} 파일을 찾을 수 없습니다.")


    try:
        with open(output_proto_path, mode='w', encoding='utf-8') as f:
            f.write('// [자동 생성된 파일입니다. 절대 직접 수정하지 마세요!]\n')
            f.write('syntax = "proto3";\n\n')
            f.write('package Protocol;\n\n')
            f.write('option csharp_namespace = "Google.Protobuf.Protocol";\n\n')
            f.write('import "google/protobuf/timestamp.proto";\n\n')

            if enums:
                f.write('enum ProtocolID {\n')
                for elem in enums:
                    f.write(f"  {elem['name']} = {elem['id']};\n")
                f.write('}\n\n')

            for msg_name, fields in messages.items():
                f.write(f'message {msg_name} {{\n')
                for field in fields:
                    rule = field['rule'] + ' ' if field['rule'] else ''
                    f.write(f" {rule}{field['type']} {field['name']} = {field['number']};\n")
                f.write('}\n\n')

        print(f" 성공: {output_proto_path} 파일이 정상적으로 생성되었습니다.")
    except Exception as e:
        print(f"파일 쓰기 에러 발생: {e}") 

def generate_header_packet_handlers():
    """
    Direction이 C2S인 패킷들을 모아서 C++ 핸들러 함수 선언부를 생성합니다.
    """

    output_code = "// 자동 생성된 패킷 핸들러 선언부입니다. 수동 편집 금지!\n"
    output_code += '#pragma once\n'

    output_code += '#include "PacketHandler.h"\n'

    output_code += "class ClientPacketHandler : public PacketHandlerBase<ClientPacketHandler> {\n"
    output_code += "public:\n"
    output_code += "    static void Init();\n"
    output_code += "private:\n"

    for msg_name in messages.keys():
        if msg_name.startswith("C2S"):
            output_code += f"    static bool Handle_{msg_name}(SessionRef& s, BYTE* b, int32_t l);\n"

    output_code += "\n"

    output_code += "};\n\n"

    with open("ClientPacketHandler.gen.h", mode='w', encoding='utf-8') as f:
        f.write(output_code)
        
    print("성공: 'ClientPacketHandler.gen.h' 파일이 생성되었습니다")

def generate_cpp_packet_handlers():
    """
    Direction이 C2S인 패킷들을 모아서 C++ 핸들러 함수 정의부를 생성합니다.
    """

    output_code = "// 자동 생성된 패킷 핸들러 정의부입니다. 수동 편집 금지!\n"

    output_code += '#include "ClientPacketHandler.gen.h"\n'
    
    output_code += 'void ClientPacketHandler::Init()\n'
    output_code += '{\n'
    for msg_name in messages.keys():
        if msg_name.startswith("C2S"):
            output_code += f'    Register(Protocol::ID_{msg_name}, Handle_{msg_name});\n'
    output_code += '}\n'

    with open("ClientPacketHandler.gen.cpp", mode='w', encoding='utf-8') as f:
        f.write(output_code)

    print("성공: 'clientPakcetHandler.gen.cpp' 파일이 생성되었습니다")

if __name__ == "__main__":
    generate_proto_from_csv('Packets - enums.csv', 'Packets - messages.csv', 'Protocol.proto')
    generate_header_packet_handlers()
    generate_cpp_packet_handlers()
    


