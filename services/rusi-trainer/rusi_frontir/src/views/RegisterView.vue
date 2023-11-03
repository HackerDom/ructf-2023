<template>
  <div class="container">
    <form class="card" @submit.prevent="registerUser">
      <div class="field">
        <input
          class="gate"
          id="username"
          type="text"
          placeholder="Как звать то?"
          v-model="username"
        /><label for="username" title="Username">Прозвище</label>
      </div>
      <div class="field">
        <input
          class="gate"
          id="password"
          type="password"
          placeholder="Защитись от ящериц поганых"
          v-model="password"
        /><label for="password" title="Password">Тайнопись</label>
      </div>
      <button class="slavic-button" type="submit" title="Register">Приобщиться</button>
      <div class="groster">
        <p class="error" title="Error" v-if="error">Недосмотр: {{ error }}</p>
        <p>
          Уже во собрании? <router-link to="/login" class="link" title="Login">Возойти</router-link>
        </p>
      </div>
    </form>
  </div>
</template>

<script>
import { registerMy } from '@/api/api'

export default {
  name: 'RegisterView',
  data() {
    return {
      username: '',
      password: '',
      error: ''
    }
  },
  methods: {
    registerUser() {
      if (!this.username) {
        this.error = 'Позабыл прозвище'
        return
      }
      if (!this.password) {
        this.error = 'Позабыл тайнопись'
        return
      }
      registerMy(this.username, this.password)
        .then(() => this.$router.push('/login'))
        .catch((error) => {
          console.log(error)
          this.error = error.message
        })
    }
  }
}
</script>

<style scoped></style>
