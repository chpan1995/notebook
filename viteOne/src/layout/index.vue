<template>
    <div class="main_container">
        <div class="layout-container">
            <template v-for="(item, index) in constantRoute" :key="item.path">
                <template v-if="!item.children"></template>
                <!-- <router-link :to="item.path">{{ item.name }}</router-link> -->
                <template v-else>
                    <template v-for="(child, cIndex) in item.children" :key="child.path">
                        <!-- <router-link :to="item.path + '/' + child.path">{{ child.name }}</router-link> -->
                         <el-button type="primary" @click="() => $router.push(item.path + '/' + child.path)">
                            {{ child.meta?.title }}
                         </el-button>
                    </template>
                </template>
            </template>
        </div>

        <router-view v-slot="{ Component }">
            <transition name="fade" mode="out-in">
                <component :is="Component" />
            </transition>
        </router-view>
    </div>
</template>

<script setup lang="ts">
import { constantRoute } from '@/router/router';
import { useRouter } from 'vue-router';
const $router = useRouter();
</script>

<style scoped lang="scss">
.main_container {
    height: 100vh;
    width: 100vw;
    display: flex;
    flex-direction: column;

    .layout-container {
        display: flex;
        gap: 20px;
        padding: 10px;
        background-color: #f0f0f0;
        justify-content: space-evenly;

        a {
            text-decoration: none;
            color: #333;
            font-weight: bold;

            &:hover {
                color: #007BFF;
            }
        }
    }
}
</style>