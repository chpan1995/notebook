<template>
    <div class="son">
        <ul>
            <li v-for="(item, index) in todo" :key="item.id">
                <slot :$raw="item" :$index="index"></slot>
            </li>
        </ul>
    </div>
</template>

<script setup lang="ts">
defineProps(['todo']);
</script>

<style scoped lang="scss">
.son {
    flex: 1;
    display: flex;
    justify-content: center;
    /* 水平居中 ul（可留可删） */
    align-items: center;
    background-color: beige;
    width: 100%;
    ul {
        list-style: none;
        padding: 0;
        font-size: 24px;
        // 如果你不写 width，ul 默认是 width: auto（≈100%），就居不中
        width: fit-content;
        /* 或固定宽度 */
        margin: 0 auto;

        li {
            display: flex;
            justify-content: center;
            /* 水平居中 */
            align-items: center;
            /* 垂直居中（可选） */
        }
    }
}
</style>