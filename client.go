package dfhack

// #cgo LDFLAGS: /home/towski/save/df_linux/hack/libprotobuf-lite.so /home/towski/save/df_linux/hack/libdfhack-client.so -lstdc++ 
// #cgo CXXFLAGS: -m32 -I library/include/df -I library/include/ -I library/proto -I depends/protobuf/google/protobuf/ -I depends/protobuf -std=c++11
// #include "dfhack.h"
import "C"
import _ "fmt"

func Connect(){
    C.Connect()
}

func Update(){
    C.Update()
}

func GetDwarf(x int) *C.struct_CDwarf{
    return C.GetDwarf(C.int(x))
}
