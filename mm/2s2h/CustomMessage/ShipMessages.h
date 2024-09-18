// clang-format off
DEFINE_MESSAGE(HELLO_WORLD, 0x00, "\x06\x00" "\xFE" "\xFF\xFF" "\xFF\xFF" "\xFF\xFF" "\xFF\xFF"
"Hello World!\x11"
"You have {{rupees}} rupee(s).\x11"
"\x11"
"                   - The Moon"
)
DEFINE_MESSAGE(GIVE_ITEM, 0x00, "\x02\x00" "\x0C" "\xFF\xFF" "\xFF\xFF" "\xFF\xFF" "\xFF\xFF"
"You recieved a(n) {{item}}!"
)
DEFINE_MESSAGE(GIVE_ITEM_NO_STOP, 0x00, "\x02\x00" "\x0C" "\xFF\xFF" "\xFF\xFF" "\xFF\xFF" "\xFF\xFF"
"{{item}}" "\x1C\x02\x10"
)
