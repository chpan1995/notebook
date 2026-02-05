<template>
    <div class="parent">
        <Test1>
            <div>
                <pre> 大奖都去 </pre>
            </div>
            <!-- 具名插槽 -->
            <template #a>
                <pre>哈哈哈</pre>
            </template>
        </Test1>

        <Test2 :todo="todo">
           <template v-slot="{$raw,$index}">
                <span :style="{ color: $raw.done ? 'green' : 'red'  }"> {{$raw.title}}--{{ $index}}</span>
           </template>
        </Test2>
    </div>

</template>

<script setup lang="ts">
import Test1 from './Test1.vue';
import Test2 from './Test2.vue';

import {ref} from 'vue'
let todo=ref(
  [
    {id:1,title:"haha",done:true},
    {id:2,title:"abcabc",done:true},
    {id:3,title:"efdefd",done:false},
  ]
);

</script>

<style scoped lang="scss">
.parent {
    display: flex;
    flex-direction: column;
    align-items: center;
    height: 100vh;
    width: 100vw;
}
</style>