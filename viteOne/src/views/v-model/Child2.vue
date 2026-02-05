<template>
    <div class="son2">
        <div>
            <el-button @click="$emit('update:pageNo', props.pageNo + 1)">pageNo</el-button>
            <el-button @click="$emit('update:pageSize', props.pageSize + 1)">pageSize</el-button>
        </div>

    </div>
</template>

<script setup lang="ts">

let props = defineProps<{
    pageNo: number,
    pageSize: number
}>();

let $emit = defineEmits(['update:pageNo', 'update:pageSize']);

</script>


<style scoped>
.son2 {
    background-color: antiquewhite;
    /* 相对于父类的比例 */
    width: 100%;
    flex: 1;
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;
}
</style>