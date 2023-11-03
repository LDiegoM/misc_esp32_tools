function saveDate() {
    const body = getUpdateBody();

    executeCall("/settings/date", "PUT", body).then((updOk) => {
        if (updOk == true) {
            document.location.reload(true);
        }
    });
}

function getUpdateBody() {
    const server1 = document.getElementById("server1").value;
    const server2 = document.getElementById("server2").value;
    const gmtOffset = document.getElementById("gmtOffset").value;
    const daylightOffset = document.getElementById("daylightOffset").value;

    var body = '{"server1":"' + server1 + '","server2":"' + server2 + '","gmt_offset":' + gmtOffset + ',"daylight_offset":' + daylightOffset + '}';
    return body;
}
