<!DOCTYPE html>
<html>
<head>
  <meta http-equiv="content-type" content="text/html; charset=UTF-8">
  <title>My Interface</title>
  
  <script type='text/javascript' src='//code.jquery.com/jquery-1.9.1.js'></script>
  
  

<script type='text/javascript'>//<![CDATA[ 

$(function () {

    $.getJSON("data.php",function(data) {    
        // Create the chart
        $('#container').highcharts('StockChart', {

           credits: {
               enabled: false
           },
           rangeSelector : {
                allButtonsEnabled: true,
                buttons: [{
                    type: 'day',
                    count: 1,
                    text: 'Day'
                }, {
                    type: 'week',
                    count: 1,
                    text: 'Week'
                }, {
                    type: 'month',
                    count: 1,
                    text: 'Month'
                 }, {
                    type: 'year',
                    count: 1,
                    text: 'Year'                    
                }, {
                    type: 'all',
                    count: 1,
                    text: 'All' 
                }],
                buttonTheme: {
                    width: 60
                },
                selected: 2
            },

            title : {
                text : data[0]['title']
            },
                legend: {
                    enabled: true,
                    layout: 'vertical',
                    align: 'right',
                    verticalAlign: 'top',
                    x: -10,
                    y: 100,
                    borderWidth: 0
                },
            xAxis: {
                type: 'datetime',
                dateTimeLabelFormats: {
                second: '%Y-%m-%d<br/>%H:%M:%S',
                minute: '%Y-%m-%d<br/>%H:%M',
                hour: '%Y-%m-%d<br/>%H:%M',
                day: '%Y<br/>%m-%d',
                week: '%Y<br/>%m-%d',
                month: '%Y-%m',
                year: '%Y'
                }
            },
            yAxis: {
                floor: 0
            },
            tooltip: {
                formatter: function () {
                     var s = '<b>' + Highcharts.dateFormat('%d %b  %H:%M:%S', this.x) + '</b>';

                     $.each(this.points, function () {
                          s += '<br/>' + this.series.name + ': ' + this.y ;
                     });

                    return s;
                }
            },
            series : [{
                name : data[0]['name1'],
                data : data[1]['data'],
                tooltip: {
                    valueDecimals: 2
                }},
                {
                name : data[0]['name2'],
                data : data[2]['data'],
                tooltip: {
                    valueDecimals: 2
                }                
            }]
        });
    });

});

//]]>  

</script>


</head>
<body>
<script src="http://code.highcharts.com/stock/highstock.js"></script>
<script src="http://code.highcharts.com/stock/modules/exporting.js"></script>

<div id="container" style="height: 400px; min-width: 310px"></div>
  
</body>

</html>

