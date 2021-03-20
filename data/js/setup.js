const label = document.getElementById('label')

const configureWifi = (e) => {
  label.innerHTML = "Connecting...";
  e.preventDefault();
  const ssid = document.getElementById('ssid').value;
  const password = document.getElementById('password').value;

  if (ssid && password) {
    label.innerHTML = "Look at the LEDs on the breadboard. Red blink = Not connected. Green blink = Connected."
    fetch('./configureWifi', {
      method: 'POST',
      body: `{\"ssid\":\"${ssid}\",\"password\":\"${password}\"}`
    })
  } else {
    label.innerHTML = "SSID and Password fields are required";
  }
}

document.getElementById('btnSubmit').addEventListener('click', configureWifi);