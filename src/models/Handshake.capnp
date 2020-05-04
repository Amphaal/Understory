 @0x95134c8da401cda1;

using import "Character.capnp".Character;
struct Handshake {
    username @0 :Text;
    clientVersion @1 :Text;
    toIncarnate @2 :Character;
}