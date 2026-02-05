<template>
    <div class="parent">
        <h1> money: {{ money }} ----  pageNo: {{ pageNo }} ---- pageSize: {{ pageSize }}</h1>
        <div class="content">
            <el-input v-model="info" placeholder="请输入内容" @change="handleChange"></el-input>
            <!-- <Child :money="money" @update:moneyValue="handel"></Child> -->
            <!--- v-model后面不写默认属性是modelValue---->
            <Child1 v-model="money"></Child1>
            <Child2 v-model:page-no="pageNo" v-model:page-size="pageSize"></Child2>
        </div>

    </div>



</template>

<script setup lang="ts">

import { ref } from 'vue';
import Child1 from './Child.vue';
import Child2 from './Child2.vue';

let info = ref('');
let money = ref(10000);
let pageNo = ref(1);
let pageSize = ref(2);

let handel = (data: any) => {
    console.log(data);
    money.value = data;
}

let handleChange = (val: any) => {
    console.log(val);
}


</script>

<style scoped lang="scss">
.parent {
    display: flex; 
    flex-direction: column; 
    height: 100vh;
    h1 {
        text-align: center;
        font-size: 28px;
        font-weight: bold;
    }
    /*这让 .content 占满剩余高度，子元素 flex:1 就能正确撑满，不需要算 header 高度，也能避免整页滚动。*/
    .content {
        display: flex;
        flex-direction: column;
        gap: 20px;
        padding: 20px;
        flex: 1;
        overflow: auto; 
    }
}
</style>