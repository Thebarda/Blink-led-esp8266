const toggleLed = () => {
  fetch('./toggleLed').then(response => response.json()).then(({ status }) => {
    document.getElementById('ledStatus').innerHTML = status ? 'ON' : 'OFF';
  })
}

document.getElementById('ledBtn').addEventListener('click', toggleLed);

fetch('./ledStatus').then(response => response.json()).then(({ status }) => {
    document.getElementById('ledStatus').innerHTML = status ? 'ON' : 'OFF';
  })