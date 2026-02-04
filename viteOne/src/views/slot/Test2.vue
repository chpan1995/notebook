<template>
    <div class="son">
        <ul>
            <li v-for="(item, index) in todo" :key="item.id" >
                <slot :$raw="item" :$index="index"></slot>
            </li>
        </ul>
    </div>
</template>

<script setup lang="ts">
defineProps(['todo']);
</script>