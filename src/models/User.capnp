@0x976344c794a95ff0;

using import "Character.capnp".Character;

struct User {
    id @0 :UInt64;
    name @1 :Text;
    type @2 :Type;
    color @3 :UInt64;
    boundCharacter @4 :Character;

    enum Type {
        observer @0;
        host @1;
        player @2;
    }
     
}