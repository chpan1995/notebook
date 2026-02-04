<template>
    <div>
        <h1>
            曹操的钱: {{ money }}
        </h1>
        <button @click="handel"> 加   钱 </button>
        <Son ref="son"></Son>
        <Daughter></Daughter>
    </div>
</template>

<script setup lang="ts">

import {ref} from 'vue';
import Son from './Son.vue';
import Daughter from './Daughter.vue';
// 获取子组件实例
let son = ref();

let money = ref<number>(1000);

const handel = ()=> {
    money.value +=  100;
    son.value.money -= 100;
    son.value.fun();
}

defineExpose({
    money
})

</script>

<style scoped lang="scss">

</style>