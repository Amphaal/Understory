@0xe300aeb264e04866;

using import "Asset.capnp".Asset;
struct Character {
    name @0 :Text;
    archtype @1 :Text;
    description @2 :Text;
    story @3 :Text;
    level @4 :Int32;
    bonus @5 :Text;
    malus @6 :Text;
    notes @7 :Text;
    abilities @8 :List(Ability);
    inventories @9 :List(Inventory);
    gauges @10 :List(Gauge);
    boundPortrait @11 :Asset;
}

struct Gauge {
    current @0 :Int64;
    min @1 :Int64;
    max @2 :Int64;
    name @3 :Text;
    color @4 :Int64;
    isVisibleUnderPortrait @5 :Bool;
}

struct Ability {
    name @0 :Text;
    category @1 :Text;
    description @2 :Text;
    isFavorite @3 :Bool;
}

struct Inventory {
    name @0 :Text;
    slots @1 :List(InventorySlot);

    struct InventorySlot {
        name @0 :Text;
        howMany @1 :Int64;
        category @2 :Text;
        description @3 :Text;
        weight @4 :Int64;
    }
}
