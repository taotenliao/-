<script setup>
import axios from 'axios';
</script>

<template>
<body>
<div class="home">
  <div class="op_window">
    <div class="chose_algorithm">
    <button @click="reInput" class="topButton">重新输入</button>
    <button @click="getData('depthFirst')" class="topButton">深度优先</button>
    <button @click="getData('widthFirst')" class="topButton">宽度优先</button>
    <button @click="getData('inspireFunc1')" class="topButton">启发策略一</button>
    <button @click="getData('inspireFunc2')" class="topButton">启发策略二</button>
    <button @click="getData('random')" class="topButton">随机扩展</button>
    </div>
  <div class = "image">
    <div class="StatusImages">
      <div v-for="layer in $data.layerMain" :key="layer" class="layer">
    <div v-for="st in layer" :key="st" class="Status"  :id="st.number" v-show="st.number<=$data.currentNumber">
      <p class="tip">值:{{st.value}}</p>
      <div class="StatusBox">
      <div v-for="line in st.status" :key="line" class="line">
      <div v-for="word in line" :key="word" class="word" :class="word.type">
        <p>{{word.value}}</p>
      </div>  
      </div>
      </div>
      <p class="tip">No.{{st.number}}</p>
    </div>
  </div>
    </div>
    <div class="connetLineS">
        <svg xmlns="http://www.w3.org/2000/svg" version="1.1" class="LinePainter">
          <line  v-for="a_line in $data.StatusNumLines" 
          :key="a_line" 
          :x1="a_line.x_father" :y1="a_line.y_father" 
          :x2="a_line.x_self" :y2="a_line.y_self" 
          :style="a_line.style" />
       </svg>

    </div>
  </div>
  <div class="right_window">
  
  <div class="algorithm_inform">
    <h2>算法描述:</h2>
    <p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;{{$data.Describe}}</p>
  </div>
  <div class="result_inform">
    <p>结果:{{$data.result.findResult}}</p>
    <p>用时:{{$data.result.useTime}}</p>
    <p>搜索节点数量:{{$data.result.node}}</p>
    <p>目标所在层数:{{$data.result.layer}}</p>
  </div>
  <div class="op_chose">
  <button @click="Begin" class="op_choseButton">动画演示</button>
  <button @click="PreviousStep" class="op_choseButton">上一步</button>
  <button @click="NextStep" class="op_choseButton">下一步</button>
  </div>
</div>
  </div>
  <div class="input_window_mask" v-show="$data.showInput">

    <div class="input_window">
    <div>
    <div><span class="chose_word">选择行</span><input type="text" v-model="$data.row"></div>
    <div><span class="chose_word">选择列</span><input type="text" v-model="$data.cl"></div>
    <div><span class="chose_word">选择层数</span><input type="text" v-model="$data.layer"></div>
    <div><button @click="choseStatus" class="op_choseButton">确认</button></div>
    <button @click="Excute" class="op_choseButton">运行</button>
  </div>
  <div>
    <p class="chose_word">起始状态</p>
    <div class="StatusBox">
      <div v-for="line in $data.starStatus" :key="line" class="line">
        <div v-for="word in line" :key="word" class="word" :class="word.type" 
        @click="choseWordStar(word)"
        @mouseenter="foucsWordStar(word)"
        @mouseleave="blurWordStar(word)"
        >
        <p>{{word.value}}</p>
      </div>  
      </div>
      </div>
  </div>
  <div>
    <p class="chose_word">终止状态</p>
    <div class="StatusBox">
      <div v-for="line in $data.endStatus" :key="line" class="line">
      <div v-for="word in line" :key="word" class="word" :class="word.type"
       @click="choseWordEnd(word)"
       @mouseenter="foucsWordEnd(word)"
       @mouseleave="blurWordEnd(word)"
       >
        <p>{{word.value}}</p>
      </div>  
      </div>
      </div>
  </div>
  </div>
  </div>
</div>

</body>
</template>
<script>

export default {
  name: 'home',
  data(){
    return{
      depthFirst:"",
      layerMain:[[]],
      row:3,
      cl:3,
      layer:7,
      StatusNumList:[],
      StatusNumLines:[],
      currentNumber:0,
      starStatus:[],
      endStatus:[],
      starValue:1,
      endValue:1,
      maxNumber:0,
      showInput:true,
      result:{findResult:"没找到",useTime:"0ms",node:-1,layer:-1},
      Describe:"",
      source:"",
      dest:"",
      charMap:"012345678QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm!@#$%^&(){}+-*?/=",
      lineStyle:[
                "stroke:rgb(0, 0, 0);stroke-width:2",
                "stroke:rgb(255, 0, 0);stroke-width:2",
                "stroke:rgb(242, 217, 13);stroke-width:2"
              ]
    }
  },
  onload(){
  },
  mounted(){
    this.screenWidth = document.body.clientWidth
    window.onresize = () => {   //屏幕尺寸变化就重新赋值
      return (() => {
        this.paintLine()
      })()
    }
  },
  methods : {
  paintLine:function(){
    this.StatusNumLines = []
    if(this.currentNumber>1){
      const StatusMap = new Map();
      let obj1 = document.getElementById(1)
      const offsetTop = obj1.offsetTop;
      const Widthh = obj1.offsetWidth;
      const Height = obj1.offsetHeight;
    for(let i=0;i<this.currentNumber;i++){//计算位置
      let obj = document.getElementById(i+1)
      StatusMap.set(i+1,{
        left:Number(obj.offsetLeft)+Widthh/2,
        up:Number(this.StatusNumList[i].depth)*(offsetTop*2+Height)+offsetTop,
        down:Number(this.StatusNumList[i].depth)*(offsetTop*2+Height)+offsetTop + Height
      })
    }
    for(let i=1;i<this.currentNumber;i++){//连线
      const father = StatusMap.get(Number(this.StatusNumList[i].father))
      const self = StatusMap.get(Number(this.StatusNumList[i].number))
      this.StatusNumLines.push({
        number:i,
        father:Number(this.StatusNumList[i].father),
        self:Number(this.StatusNumList[i].number),
        x_father:parseInt(father.left),
        y_father:parseInt(father.down),
        x_self:parseInt(self.left),
        y_self:parseInt(self.up),
        style:this.$data.lineStyle[0]
      })
    }
    //寻找路径
    const lineStyle = this.currentNumber==this.StatusNumList.length?this.$data.lineStyle[2]:this.$data.lineStyle[1]
    let current_father = this.StatusNumLines[this.currentNumber-2].father
    this.StatusNumLines[this.currentNumber-2].style = lineStyle
    for(let i=this.currentNumber-1;i>0;i--){
      if(this.StatusNumLines[i-1].self == current_father){
        current_father = this.StatusNumLines[i-1].father
        this.StatusNumLines[i-1].style = lineStyle
      }
    }
   }
  },
  Begin:function(){
    this.currentNumber = 1
    this.$nextTick(() => {
            this.paintLine()
        })
  },
  PreviousStep:function(){
    if( this.currentNumber > 1){
      this.currentNumber-=1
      this.$nextTick(() => {
            this.paintLine()
        })
    }
  },
  NextStep:function(){
    if( this.currentNumber < this.StatusNumList.length){
      this.currentNumber+=1
      this.$nextTick(() => {
            this.paintLine()
        })
    }
  },
  Excute:function(){
    if(this.$data.endValue==this.$data.maxNumber&&this.$data.starValue==this.$data.maxNumber){
      const that = this;
      axios.post("http://127.0.0.1:8085/eight",{
        "row":that.$data.row,
        "cl":that.$data.cl,
        "layer":that.$data.layer,
        "source":that.$data.source,
        "dest":that.$data.dest
      }).then(response=>{
        console.log(response)
      })
      this.$data.showInput = false
      this.$data.starStatus=[]
      this.$data.endStatus=[]
      this.$data.starValue=1
      this.$data.endValue=1
      this.$data.maxNumber=0
    }
    else {
      alert("状态不完整！")
    }
  },
    getData:function(Type){
	      //  更新数据market_id.txt文件接口
	      let xhr = new XMLHttpRequest(),
	      okStatus = document.location.protocol === "file:" ? 0 : 200;
        let xhr2 = new XMLHttpRequest();
        let xhr3 = new XMLHttpRequest();
          switch(Type){
            case 'depthFirst':
              xhr.open("GET", "depthFirst.txt", false); // 搜索节点文件路径
              xhr2.open("GET", "depthFirstSearchInform.txt", false); // 搜索结论文件路径
              xhr3.open("GET", "depthFirstDescribe.txt", false); // 搜索描述文件路径
            break;
            case 'widthFirst':
              xhr.open("GET", "widthFirst.txt", false); // 文件路径
              xhr2.open("GET", "widthFirstSearchInform.txt", false); // 文件路径
              xhr3.open("GET", "widthFirstDescribe.txt", false); // 搜索描述文件路径
            break;
            case 'inspireFunc1':
              xhr.open("GET", "inspireFunc1.txt", false); // 文件路径
              xhr2.open("GET", "inspireFunc1SearchInform.txt", false); // 文件路径
              xhr3.open("GET", "inspireFunc1Describe.txt", false); // 搜索描述文件路径
            break;
            case 'inspireFunc2':
              xhr.open("GET", "inspireFunc2.txt", false); // 文件路径
              xhr2.open("GET", "inspireFunc2SearchInform.txt", false); // 文件路径
              xhr3.open("GET", "inspireFunc2Describe.txt", false); // 文件路径
            break;
            case 'random':
              xhr.open("GET", "random.txt", false); // 文件路径
              xhr2.open("GET", "randomSearchInform.txt", false); // 文件路径
              xhr3.open("GET", "randomDescribe.txt", false); // 文件路径
              break;
            default:
            break;
          }
	      xhr.overrideMimeType("text/html;charset=utf-8"); //默认为utf-8
	      xhr.send(null);
        xhr2.overrideMimeType("text/html;charset=utf-8"); //默认为utf-8
	      xhr2.send(null);
        xhr3.overrideMimeType("text/html;charset=utf-8"); //默认为utf-8
	      xhr3.send(null);
        console.log(okStatus) 
        this.layerMain = [[]]
        this.StatusNumList=[]
        this.StatusNumber = 0
        let list = xhr.responseText.split(" ")
        let list2 = xhr2.responseText.split(" ")
        this.Describe = xhr3.responseText
        console.log(list2)
        this.result.layer = list2[0]
        this.result.node = list2[1]
        this.result.useTime = list2[2]+"ms"
        if(list2[0]==-1){
           this.result.findResult = "没找到"
          }
        else{
           this.result.findResult = "已找到"
          }
        for(let i = 0;i<list.length-1;i+=5){
          this.layerMain.push([])
        }
        console.log(list)
        for(let i = 0;i<list.length-1;i+=5){
          let str = list[i]
          let listchar=[]
          const len = str.length/this.cl
          for(let j=0;j<len;j+=1){
            let line=[]
            for(let u=0;u<this.cl;u++){
              if(str[j*this.cl+u]!='*'){line.push({value:this.$data.charMap.indexOf(str[j*this.cl+u]),type:'number'})}
              else(line.push({value:str[j*this.cl+u],type:'move'}))
            }
            listchar.push(line)
          }
          this.layerMain[Number(list[i+2])].push({status:listchar,value:list[i+1],depth:list[i+2],number:list[i+3],father:list[i+4]})
          this.StatusNumList.push({depth:Number(list[i+2]),number:list[i+3],father:list[i+4]})
        }
        this.currentNumber = this.StatusNumList.length
        console.log(this.currentNumber)
        this.$nextTick(() => {
            this.paintLine()
        })
  },
  choseStatus:function(){
    this.starStatus=[]
    this.endStatus=[]
    this.source = ""
    this.dest = ""
    this.$data.endValue = 1
    this.$data.starValue = 1
    for(let i=0;i<this.row;i++){
      this.starStatus.push([])
      this.endStatus.push([])
      for(let j=0;j<this.cl;j++){
        this.source+='*'
        this.dest+='*'
        this.starStatus[i].push({value:'*',type:'move',index:i*this.cl+j})
        this.endStatus[i].push({value:'*',type:'move',index:i*this.cl+j})
      }
    }
    this.$data.maxNumber = this.$data.row  * this.$data.cl
  },
  choseWordStar:function(word){
    if(word.value=='*'&&this.$data.starValue<this.$data.maxNumber){
    word.value = this.$data.starValue
    this.$data.source = this.$data.source.substring(0,word.index) 
    + this.$data.charMap[word.value]
    + this.$data.source.substring(word.index + 1,this.$data.source.length) 
    this.$data.starValue++
    word.type = 'number'
    console.log(this.$data.source)
    }
    
  },
  foucsWordStar:function(word){
    if(word.value=='*'&&this.$data.starValue<this.$data.maxNumber){
      word.type = 'foucs'
    }
  },
  blurWordStar:function(word){
    if(word.value=='*'&&this.$data.starValue<this.$data.maxNumber){
      word.type = 'move'
    }
  },
  foucsWordEnd:function(word){
    if(word.value=='*'&&this.$data.endValue<this.$data.maxNumber){
      word.type = 'foucs'
    }
  },
  blurWordEnd:function(word){
    if(word.value=='*'&&this.$data.endValue<this.$data.maxNumber){
      word.type = 'move'
    }
  },
  choseWordEnd:function(word){
    if(word.value=='*'&&this.$data.endValue<this.$data.maxNumber){
    word.value = this.$data.endValue
    this.$data.dest = this.$data.dest.substring(0,word.index) 
    + this.$data.charMap[word.value]
    + this.$data.dest.substring(word.index + 1,this.$data.dest.length) 
    this.$data.endValue++
    word.type = 'number'
  }
  },
  reInput:function(){
    this.$data.showInput = true
  }
    
}
}
</script>
<style scoped>
/*全局去除默认样式*/ 
*{
	padding:0;
	margin:0;
  list-style:none;
  align-items: center;
}

.tip {
  color: black;
  font-size: 1em;
}
.line {
  display: flex;
  width: 100%;
  height: 100%;
}
.word {
  color: black;
  border: 1px solid black;
  font-size: 100%;
  width: 100%;
  height: 100%;
  display: flex;
  flex-flow:column;
}
.home {
  width: 100vw;
  height: 100vh;
  background-image: url("../img/bg.jpg");
  background-size:cover;
  float: left;
}
.layer {
  display: flex;
  flex-flow:row;
  justify-content:center;
}
.Status {
  width: 18vh;
  height: 20vh;
  border: 2px solid black;
  border-radius: 5px;
  margin-top: 25px;
  margin-bottom: 25px;
  display: flex;
  flex-flow:column;
  justify-content:center;
  align-items: center;
  background-color: white;
}
.number {
  background-color: rgb(55, 196, 240);
}
.move{
  background-color: rgb(20, 18, 176);
}
.foucs{
  background-color: rgb(255, 0, 0);
}
.op_window {
  position: absolute;
  z-index: 5;
  width: 80vw;
  height: 90vh;
  background-color: rgb(107, 181, 222);
  margin-left: 10vw;
  margin-top: 5vh;
}
.StatusBox{
  width: 14vh;
  height: 14vh;
  border: 3px solid gray;
  overflow: hidden;
  display: flex;
  flex-flow:column;
  justify-content:center;
}
.connetLineS,.LinePainter{
  width: 200vw;
  height: 200vh;
  z-index: 10;
  position:absolute;
}
 .image {
  width: 80%;
  height: 95%;
  position:absolute;
  overflow: scroll;
 } 
 .right_window {
  width: 20%;
  height: 95%;
  margin-left: 80%;
  background-color: rgb(20, 18, 176);
  position:absolute;
 }
 .algorithm_inform {
  width: 100%;
  height: 45%;
  background-color: rgb(107, 181, 222);
 }
 .result_inform {
  height: 15%;
  background-color: rgb(55, 196, 240);
 }
 .op_chose {
  width: 100%;
  height: 40%;
  background-color: rgb(20, 18, 176);
  display: flex;
  flex-flow:column;
  justify-content:center;
 }
 .chose_algorithm {
  width: 100%;
  height: 5%;
  background-color: rgb(20, 18, 176);
 }
.StatusImages {
  z-index: 9;
  width: 100%;
  position:absolute;
}
.input_window_mask {
  width: 100vw;
  height: 100vh;
  z-index: 10;
  position: absolute;
  display: flex;
  flex-flow:row;
  justify-content:center;
  align-items: center;
}
.input_window {
  width: 70vw;
  height: 50vh;
  z-index: 10;
  position: absolute;
  background-color: rgba(0,0,0,0.7);
  display: flex;
  flex-flow:row;
  justify-content:center;
  align-items: center;
}
.chose_word {
  color: white;
  width: 10vw;
  height: 100%;
  display: flex;
  justify-items: center;
  align-items: center;
}
.topButton {
  height: 100%;
}
.op_choseButton {
  width: 8vw;
  height: 4vh;
}
</style>
