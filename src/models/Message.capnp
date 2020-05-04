@0xdcb4c422ba80c8d2;

using import "User.capnp".User;
struct Message {
    sender @0 :User;
    content @1 :Text;
}

struct Response {
    responseTo @0 :Message;
    content :union {
        unknownCommand @1 :Void;
        connectedToServer @2 :User;
        error @3 :Text;
        errorRecipients @4 :List(User);
        ack @5 :Void;
        helpManifest @6 :Text;
        status @7 :Text;
    }
}