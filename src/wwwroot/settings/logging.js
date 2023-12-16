function saveLogging() {
    const body = getUpdateBody();

    executeCall("/settings/logging", "PUT", body).then((updOk) => {
        if (updOk == true) {
            document.location.reload(true);
        }
    });
}

function getUpdateBody() {
    const level = document.getElementById("level").value;
    console.log("level: " + level);
    const refresh_period = document.getElementById("refresh_period").value;

    return '{"level":' + level + ',"refresh_period":' + refresh_period + '}';
}
