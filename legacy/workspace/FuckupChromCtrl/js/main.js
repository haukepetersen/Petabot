var uart;
var uart_id = 0;

var test = Uart;

var chart;

$(document).ready(function () {
  console.log("foo");
  
  uart = chrome.serial;
  
  uart.getPorts(function(ports) {
	console.log(ports);
  });
  
  
  uart.open('COM3', {bitrate: 115200}, function(info) {
	console.log(info);
	if (info != 0) {
		uart_id = info.connectionId;
		console.log(uart_id);
	}	
  });
  
  
  if (test.isConnected()) {
	console.log("Uart is connected");
  } else {
	console.log("Uart is not connected");
  }
  
  
  $("#disconnect").click(function() {
	  console.log("clicked on disconnect");
		if (uart_id > 0) {
		console.log('Closing connection');
		uart.close(uart_id, function(result) {
			console.log('connection closed: ');
			console.log(result);
		});
		}
	});
  
  
	
    
	chart = new Highcharts.Chart({
		chart: {
			renderTo: 'graphs_acc',
			type: 'line',
			margin: [70, 50, 60, 80],
			// events: {
				// // click: function(e) {
					// // // find the clicked values and the series
					// // var x = e.xAxis[0].value,
						// // y = e.yAxis[0].value,
						// // series = this.series[0];

					// // // Add it
					// // series.addPoint([x, y]);

				// // }
			// }
			animation: false
		},
		title: {
			text: 'User supplied data'
		},
		subtitle: {
			text: 'Click the plot area to add a point. Click a point to remove it.'
		},
		xAxis: {
			minPadding: 0,
			maxPadding: 0,
			maxZoom: 60
		},
		yAxis: {
			title: {
				text: 'Value'
			},
			minPadding: 0.2,
			maxPadding: 0.2,
			maxZoom: 60,
			plotLines: [{
				value: 0,
				width: 1,
				color: '#808080'
			}]
		},
		legend: {
			enabled: false
		},
		exporting: {
			enabled: false
		},
		plotOptions: {
			series: {
				lineWidth: 1,
				point: {
					events: {
						'click': function() {
							if (this.series.data.length > 1) this.remove();
						}
					}
				}
			}
		},
		series: [{
			data: [[1, 20], [2, 80], [3, 81]]
		}]
	});
	
	var time = 4;
	var lastval = Math.floor(Math.random() * 100);
	var interval = 0;
	
	$("#btn_start").click(function() {
		interval = setInterval(function() {
			series = chart.series[0];
			lastval = lastval + Math.floor((Math.random() * 20) - 9.5);
			point = [time++, lastval];
			if (series.data.length >= 60) {
				series.addPoint(point, true, true);
			} else {
				series.addPoint(point, true, false);
			}
		}, 10);
	});
	
	$("#btn_stop").click(function() {
		clearInterval(interval);
		console.log("stopped");
	});
	
});

