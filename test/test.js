let lib = require("../build/vwbc.js");
let uuid = require("uuid");

lib().then((vwbc) => {
    vwbc.init();
    vwbc.certificate_builder((builder) => {
        builder.add_int(0x100, 0xCAFE);
        builder.add_string(0x200, "this is a dumb way to pass messages");
        builder.add_uuid(0x300, 'f0930120-1f94-4a69-897b-50a99d2a0f32');
        builder.add_date(0x400, new Date(2019, 12, 12));
        let bytes = builder.emit();
        let hex = Buffer.from(bytes).toString('hex');
        console.log(hex);
    });
})