function saveDate(){const e=getUpdateBody();executeCall("/settings/date","PUT",e).then(e=>{if(e==true){document.location.reload(true)}})}function getUpdateBody(){const e=document.getElementById("server1").value;const t=document.getElementById("server2").value;const n=document.getElementById("gmtOffset").value;const o=document.getElementById("daylightOffset").value;var d='{"server1":"'+e+'","server2":"'+t+'","gmt_offset":'+n+',"daylight_offset":'+o+"}";return d}