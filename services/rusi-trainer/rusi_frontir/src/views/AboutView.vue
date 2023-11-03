<template>
  <div class="about">
    <h1 title="Collection of great techniques of ancient Rus">
      Собрание великих приёмов <br />
      древних русов
    </h1>
    <div class="buttons-group" v-if="!token">
      <router-link to="/login" class="slavic-button slavic-button__lg" tag="button" title="Login">
        Возойти
      </router-link>
      <router-link
        to="/register"
        class="slavic-button slavic-button__lg"
        tag="button"
        title="Register"
      >
        Приобщиться
      </router-link>
    </div>
    <div class="buttons-group" v-if="token">
      <router-link
        to="/teach"
        class="slavic-button slavic-button__lg"
        tag="button"
        title="Teach new"
      >
        Научить
      </router-link>
      <router-link
        to="/find"
        class="slavic-button slavic-button__lg"
        tag="button"
        title="Learn exists"
      >
        Обучиться
      </router-link>
    </div>
    <a class="small-link" v-if="token" @click="$emit('onlogin', null)" href="#" title="Logout"
      >Покинуть</a
    >
  </div>
</template>

<script>
export default {
  name: 'AboutView',
  props: ['token']
}
</script>
<style>
.buttons-group {
  display: flex;
  gap: 20px;
}

.about {
  font-size: 50px;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  gap: 20px;
  min-height: calc(100vh - 250px);
}
.about h1 {
  text-align: center;
  max-width: 80vw;
  text-shadow:
    1px 1px 2px red,
    0 0 1em white,
    0 0 0.2em blue;
}

a.small-link {
  font-size: 17px;
  color: black;
  margin-top: -10px;
  margin-left: -20px;
  padding: 0 5px;
}

@media (min-width: 1920px) {
  .about {
    gap: 40px;
  }
}
@media (max-width: 1920px) {
  .about {
    gap: 20px;
  }
  body {
  }
}
</style>
