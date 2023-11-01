<script setup>
import { ref } from 'vue'
import LoginView from "@/views/LoginView.vue";
import HomeView from "@/views/HomeView.vue";
import AncestorsView from "@/views/AncestorsView.vue";

const showSidebar = ref(false)
const page = ref(1)
</script>

<template>
  <div id="background-image"></div>
  <VaLayout style="height: 100vh" class="main-layout">
    <template #top>
      <VaNavbar color="rgba(255, 255, 255, 0.0)" class="py-2">
        <template #left>
          <VaButton color="rgba(255, 255, 255, 0)" :icon="showSidebar ? 'menu_open' : 'menu'" @click="showSidebar = !showSidebar"/>
        </template>
        <template #center>
          <va-navbar-item class="font-bold text-lg">
            HYPERBOREA-LEGENDS
          </va-navbar-item>
        </template>
      </VaNavbar>
    </template>

    <template #left>
        <VaSidebar v-model="showSidebar" class="py-4">
          <VaSidebarItem :active="page === 1" @click="page = 1">
            <VaSidebarItemContent>
              <VaSidebarItemTitle>
                <router-link to="/" class="link">Home</router-link>
              </VaSidebarItemTitle>
            </VaSidebarItemContent>
          </VaSidebarItem>
          <VaSidebarItem :active="page === 2" @click="page = 2">
            <VaSidebarItemContent>
              <VaSidebarItemTitle>
                <router-link to="/ancestors" class="link">Ancestors</router-link>
              </VaSidebarItemTitle>
            </VaSidebarItemContent>
          </VaSidebarItem>
          <VaSidebarItem :active="page === 3" @click="page = 3">
            <VaSidebarItemContent>
              <VaSidebarItemTitle>
                <router-link to="/login" class="link">Login</router-link>
              </VaSidebarItemTitle>
            </VaSidebarItemContent>
          </VaSidebarItem>
          <VaSidebarItem :active="page === 4" @click="page = 4">
            <VaSidebarItemContent>
              <VaSidebarItemTitle>
                <router-link to="/register" class="link">Register</router-link>
              </VaSidebarItemTitle>
            </VaSidebarItemContent>
          </VaSidebarItem>
        </VaSidebar>
    </template>

    <template #content>
      <main v-if="page === 1" class="p-4">
        <HomeView></HomeView>
      </main>
      <main v-if="page === 2" class="p-4">
        <AncestorsView></AncestorsView>
      </main>
      <main v-else-if="page === 3" class="p-4">
        <LoginView></LoginView>
      </main>
      <main v-else-if="page === 4" class="p-4">

      </main>
    </template>
  </VaLayout>
</template>

<style scoped>
@keyframes backgroundEffect {
  from {
    transform: scale(1);
  }
  to {
    transform: scale(1.5);
  }
}

#background-image {
  position: fixed;
  left: 0;
  right: 0;
  display: block;
  width: 100%;
  height: 100%;
  background-image: url(./assets/background.jpg);
  background-size: cover;
  background-position: center;
  filter: blur(2px) grayscale(70%);
  //animation: backgroundEffect 30s linear infinite alternate;
}

.main-layout {
  background-color: rgba(0, 0, 0, 0.5);
  color: #f2f2f2;
}
</style>
