/*
('.temperature__data')
('.humidity__data')
('.airQuality__data')
*/

function getData() {
  fetch("https://aumetio-default-rtdb.firebaseio.com/data/.json")
    .then((res) => res.json())
    .then((data) => {
      const arr = Object.values(data);
      $(".temperature__data").html(
        arr[arr.length - 1].Temperature.toString() + " C"
      );
      $(".humidity__data").html(arr[arr.length - 1].Humidity.toString() + " %");
      $(".airQuailty__data").html(
        Math.abs(arr[arr.length - 1].AirQuality).toString() + " ug/m(3)"
      );
    });
}

$(document).ready(function () {
  getData();
  setInterval(() => {
    getData();
  }, 2000);
});
