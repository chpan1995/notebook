<template>
    <div class="child2">
        {{ todoStore.arr }}---{{todoStore.total}}---{{todoStore.tt}}
        <button @click="()=>{ todoStore.updateTodo(); }">{{todoStore.todos}}</button>
    </div>
</template>

<script setup lang="ts">
//引入组合式API函数仓库
import useTodoStore from "@/store/modules/todo";
let todoStore = useTodoStore();
</script>


<style scoped>
.child2 {
  width: 200px;
  height: 200px;
  background: hotpink;
}
</style>