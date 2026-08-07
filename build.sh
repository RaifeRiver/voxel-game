mkdir -p bin

ODIN_FLAGS="-collection:common=src/common -strict-style"

if [ "$1" = "release" ]; then
  echo Release mode
  ODIN_FLAGS+=" -o:speed"
elif [ "$1" = "debug" ]; then
  echo Debug mode
  ODIN_FLAGS+=" -o:none -debug"
else
  echo "Usage: ./build.sh [release|debug] [client|server|all]"
  exit 1
fi

build_client() {
  echo Building client...
  odin build src/client $ODIN_FLAGS -out:bin/client
}

build_server() {
  echo Building server...
  odin build src/server $ODIN_FLAGS -out:bin/server
}

if [ "$2" = "client" ]; then
  build_client
elif [ "$2" = "server" ]; then
  build_server
elif [ "$2" = "all" ]; then
  build_client
  build_server
else
  echo "Usage: ./build.sh [release|debug] [client|server|all]"
  exit 1
fi