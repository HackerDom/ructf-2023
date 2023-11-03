<template>
  <div class="udar-viewer">
    <div class="not-found-error" v-if="!found">
      <h1 class="not-found" title="Not found item">Нет такого удара в собрании вашем</h1>
      <h2 class="not-found-help" title="Create it item">
        <router-link to="/teach">Обучите</router-link> этому приёму молодцев наших
      </h2>
    </div>
    <div class="udar" v-if="udar">
      <img
        :src="items[udar.map]"
        class="bg"
        @click="this.$emit('stopphonk')"
        alt="ПОШАГОВАЯ ИНСТРУКЦИЯ КАК СОТВОРИТЬ ВЕЛИКИЙ УДАР"
      />
      <h1 class="name-udar">{{ udar.name }}</h1>

      <div class="card">
        <h1>
          Фонк для исполения: <span class="phonk">{{ udar.phonk }}</span>
        </h1>
        <p class="desription">{{ udar.phonk }}</p>
      </div>
    </div>
    <router-link
      class="slavic-button"
      tag="button"
      title="Change item"
      :to="{ path: '/teach', query: { name: udar.name } }"
      v-if="udar && canEdit"
    >
      Подправить
    </router-link>
  </div>
</template>

<script>
import image0 from '@/assets/udars/0.jpg'
import image1 from '@/assets/udars/1.jpg'
import image2 from '@/assets/udars/2.jpg'
import image3 from '@/assets/udars/3.jpg'
import image4 from '@/assets/udars/4.jpg'
import image5 from '@/assets/udars/5.jpg'
import image6 from '@/assets/udars/6.jpg'
import { getUdar, parseJwt } from '@/api/api'

export default {
  name: 'UdarView',
  props: ['token'],
  data() {
    return {
      items: [image0, image1, image2, image3, image4, image5, image6],
      udar: null,
      found: true,
      canEdit: false
    }
  },
  mounted() {
    const name = this.$route.query.name
    if (!name) {
      this.found = false
      return
    }
    getUdar(this.token, name)
      .then((udar) => {
        this.udar = udar
        const data = parseJwt(this.token)
        console.log(data)
        if (udar.teacherId === data.userId) {
          this.canEdit = true
        }
        this.$emit('playphonk')
      })
      .catch((err) => {
        if (err.message === 'Auth error') {
          this.$emit('onlogin', null)
        }
        console.log(err)
        this.found = false
      })
  }
}
</script>

<style scoped>
.not-found {
  text-align: center;
  font-size: 60px;
}
.not-found-help {
  text-align: center;
  font-size: 40px;
}
.udar {
  position: relative;
  width: 100%;
  text-align: center;

  justify-content: center;
  align-content: space-between;
  flex-direction: column;
}
.udar .bg {
  position: relative;
  top: 0;
  margin-left: auto;
  margin-right: auto;
  right: 0;
  left: 0;

  max-height: calc(100vh - 250px);
}
.card {
  width: auto;
  margin-bottom: 5px;
  gap: 10px;
  padding: 5px;
}
.name-udar {
  color: white;
  text-shadow: black 2px 2px;
  position: absolute;
  top: 0;
  margin-left: auto;
  font-size: 40px;
  right: 0;
  left: 0;
  margin-right: auto;
}
.phonk {
  text-shadow:
    1px 1px 2px red,
    0 0 1em white,
    0 0 0.2em green;
  -webkit-animation: glowing-text 5s ease-in-out infinite alternate;
  -moz-animation: glowing-text 5s ease-in-out infinite alternate;
  animation: glowing-text 5s ease-in-out infinite alternate;
}
a.slavic-button {
  display: inline-block;
}
</style>
