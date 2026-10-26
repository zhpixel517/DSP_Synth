# TODO This code should convert a .raw audio file to a hex array for messing with using the Teensy audio library 
# TODO Change .wav audio file to .raw audio file and find the filepath (might need to put in this folder for ease)

with open("/Users/rjrae/output.raw", "rb") as file:
    data = file.read()

hex_array = [f"0x{int.from_bytes(data[i:i+2], "little", signed = True) & 0xFFFF:04X}" for i in range(0, len(data), 2)]

print(hex_array)