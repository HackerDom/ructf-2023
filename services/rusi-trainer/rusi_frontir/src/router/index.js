import { createRouter, createWebHistory } from 'vue-router'
import AboutView from '@/views/AboutView.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'home',
      component: AboutView
    },
    {
      path: '/register',
      name: 'register',
      component: () => import('../views/RegisterView.vue')
    },
    {
      path: '/login',
      name: 'login',
      component: () => import('../views/LoginView.vue')
    },
    {
      path: '/teach',
      name: 'teach',
      component: () => import('../views/TeachView.vue')
    },
    {
      path: '/find',
      name: 'learn',
      component: () => import('../views/LearnPage.vue')
    },
    {
      path: '/learn-udar',
      name: 'learnItem',
      component: () => import('../views/UdarView.vue')
    },
    {
      path: '/drinker',
      name: 'voda',
      component: () => import('../views/VodaView.vue')
    }
  ]
})

export default router
