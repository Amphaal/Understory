@0xda954d50b7eacd35;

struct SharedDocument {
  name @0 :Text;
  fileExtension @1 :Text;
  fnv1aHash @2 :UInt64;
  file @3 :Data;
  documentBytesSize @4 :UInt64;
}