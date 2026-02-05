<template>
    <div class="custom-event-root">
        <h1>事件</h1>
        <el-button class="btntest" @click="handel"> 常规按钮 </el-button>
        <Event1 @click="handle2(1,2)">  </Event1>
        <Event2 @xxx="handle3" @click="handle3"></Event2>
    </div>
</template>

<script setup lang="ts">

import Event1 from './Event.vue';
import Event2 from './Event2.vue';

const handel = ()=>{
    console.log(123);
}

const handle2 = (parm1:any,parm2:any)=>{
    console.log(parm1,parm2);
}

const handle3 = (parm1:any)=>{
    console.log(parm1);
}

</script>

<style scoped lang="scss">
.custom-event-root{
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;
    row-gap: 20px;
    width: 100vw;
    height: 100vh;
    .btntest{
        margin-bottom: 20px;
    }
    h1 {
        text-align: center;
    }
}
</style>