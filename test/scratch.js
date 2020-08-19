let lib = require("../build/vwbc.js");
let uuid = require("uuid");

lib().then((vwbc) => {
    vwbc.init();
})