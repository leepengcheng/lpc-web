<template>
  <div id="main" style="height: 400px"></div>
</template>

<script>
let echarts = require('echarts/lib/echarts')
require('echarts/lib/chart/radar')
require('echarts/lib/chart/pie')

function initData1() {
  var res = [];
  var len = 0;
  while (len++ < 3) {
    res.push({
      name: "data" + len,
      value: [
        Math.round(Math.random() * 100),
        Math.round(Math.random() * 100),
        Math.round(Math.random() * 100),
        Math.round(Math.random() * 100),
        Math.round(Math.random() * 100)
      ]
    });
  }
  return res;
}

function initData2() {
  var res = [];
  var len = 0;
  while (len++ < 3) {
    res.push({
      name: "data" + len,
      value: [
        Math.round(Math.random() * 100),
        Math.round(Math.random() * 100),
        Math.round(Math.random() * 100),
        Math.round(Math.random() * 100),
        Math.round(Math.random() * 100)
      ]
    });
  }
  return res;
}

export default {
  name: "mychart",
  data() {
    return {};
  },
  methods: {},
  mounted() {
    let myChart = echarts.init(document.getElementById("main"));
    let option = {
      title: {
        text: "动态数据",
        subtext: "纯属虚构"
      },
      tooltip: {
        trigger: "item"
      },
      legend: {
        data: ["随机数据1", "随机数据2", "随机数据3", "随机数据4", "随机数据5"]
      },
      toolbox: {
        show: true,
        feature: {
          mark: { show: true },
          dataView: { show: true, readOnly: false },
          restore: { show: true },
          saveAsImage: { show: true }
        }
      },
      polar: [
        {
          indicator: [
            { text: "指标一" },
            { text: "指标二" },
            { text: "指标三" },
            { text: "指标四" },
            { text: "指标五" }
          ],
          center: [document.getElementById("main").offsetWidth - 250, 225],
          radius: 100
        }
      ],
      calculable: false,
      series: [
        {
          name: "pie",
          type: "pie",
          radius: [0, 110],
          center: [250, 225],
          data: (function() {
            var res = [];
            var len = 0;
            while (len++ < 5) {
              res.push({
                name: "随机数据" + len,
                value: Math.round(Math.random() * 10)
              });
            }
            return res;
          })()
        },
        {
          name: "radar",
          type: "radar",
          itemStyle: { normal: { areaStyle: { type: "default" } } },
          data: (function() {
            var res = [];
            var len = 0;
            while (len++ < 3) {
              res.push({
                name: "data" + len,
                value: [
                  Math.round(Math.random() * 100),
                  Math.round(Math.random() * 100),
                  Math.round(Math.random() * 100),
                  Math.round(Math.random() * 100),
                  Math.round(Math.random() * 100)
                ]
              });
            }
            return res;
          })()
        }
      ]
    };
    myChart.setOption(option);
    // this.$nextTick(function() {
      setInterval(function() {
        option.series[0].data = initData1();
        option.series[1].data = initData2();
        myChart.setOption(option);
      }, 50);
    // });
  }
};
</script>