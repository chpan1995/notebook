<template>
    <div class="custom-event-root">
        <h1>事件</h1>
        <pre @click="handel"> test </pre>
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

</style>