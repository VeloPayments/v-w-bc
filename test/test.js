let lib = require("../build/vwbc.js");
let uuid = require("uuid");

lib().then((vwbc) => {
    vwbc.init();
    vwbc.CertificateBuilder.use((builder) => {
        builder.add_int(0x100, 0xCAFE);
        builder.add_int(0x100, 0xBABE);
        builder.add_string(0x200, "this is a dumb way to pass messages");
        builder.add_uuid(0x300, 'f0930120-1f94-4a69-897b-50a99d2a0f32');
        builder.add_date(0x400, new Date(2019, 12, 12));
        let bytes = builder.emit();
        let hex = Buffer.from(bytes).toString('hex');
        console.log(hex);
    });
    let cert = "010000040000cafe020000237468697320697320612064756d622077617920746f2070617373206d6573736167657303000010f09301201f944a69897b50a99d2a0f3204000008000000005e1a6180";
    let bytes = Buffer.from(cert, "hex");
    vwbc.CertificateReader.parse(bytes, false, (reader) => {
        console.log(reader.getFirst(0x200).string());
    });
})