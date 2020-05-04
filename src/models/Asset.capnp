@0x96fca351cb5a9a02;

struct Asset {
  name @0 :Text;
  fileExtension @1 :Text;
  fnv1aHash @2 :UInt64;
  file @3 :Data;
  struct Size {
    width @0 :UInt32;
    height @1 :UInt32;
  }
}