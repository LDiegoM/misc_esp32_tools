function saveDevice() {
    const body = getUpdateBody();

    executeCall("/settings/device", "PUT", body).then((updOk) => {
        if (updOk == true) {
            document.location.reload(true);
        }
    });
}

function getUpdateBody() {
    const deviceID = document.getElementById("deviceID").value;
    const geoLocationS = document.getElementById("geoLocationS").value;
    const geoLocationW = document.getElementById("geoLocationW").value;

    var body = '{"deviceID":"' + deviceID + '"';
    if (geoLocationS != "" && geoLocationW != "") {
        body += ',"geoLocationS":' + geoLocationS + ',"geoLocationW":' + geoLocationW;
    }
    body += '}';
    return body;
}
