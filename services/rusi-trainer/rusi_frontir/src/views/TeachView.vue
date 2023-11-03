<template>
  <div class="container teach">
    <form class="card" @submit.prevent="teachNew">
      <div class="field">
        <input
          class="gate"
          id="name"
          type="text"
          placeholder="Название для великого удара"
          v-model="form.name"
        /><label for="name" title="Name">Имечко</label>
      </div>
      <div class="field">
        <input
          class="gate"
          id="phonk"
          type="text"
          placeholder="Православный фонк для исполнения"
          v-model="form.phonk"
        /><label for="phonk" title="Phonk">Фонк</label>
      </div>
      <div class="field">
        <input
          class="gate"
          id="description"
          type="text"
          placeholder="Поясни люду, как велкийи приём делается"
          v-model="form.description"
        /><label for="description" title="Description">Описание</label>
      </div>
      <div class="field">
        <label title="Image for udar">Показ удара</label>
        <va-carousel class="carousel" v-model="form.map" :items="items" />
      </div>
      <div class="field extra">
        <p class="helper" title="Students ids">Добавь учеников по грамоте его</p>
        <input
          class="gate"
          id="trusted"
          type="text"
          placeholder="Каких молодцев обучать будите?"
          disabled
          :value="computedStr"
        /><label for="trusted" title="Students">Ученики</label>

        <div class="adder" v-if="form.trustedRusi.length < 3">
          <input id="newRus" type="number" placeholder="Номер его" v-model="newId" />
          <button
            class="slavic-button smaller"
            type="button"
            title="Добавить ученика"
            @click="newTrusted"
          >
            +
          </button>
        </div>
      </div>

      <div class="groster">
        <button class="slavic-button" type="submit" title="Teach udar">Обучить удару</button>
        <p class="error" title="Error" v-if="error">Недосмотр: {{ error }}</p>
      </div>
    </form>
  </div>
</template>

<script>
import { getUdar, teachUdar } from '@/api/api'
import image0 from '@/assets/udars/0.jpg'
import image1 from '@/assets/udars/1.jpg'
import image2 from '@/assets/udars/2.jpg'
import image3 from '@/assets/udars/3.jpg'
import image4 from '@/assets/udars/4.jpg'
import image5 from '@/assets/udars/5.jpg'
import image6 from '@/assets/udars/6.jpg'
export default {
  name: 'TeachView',
  props: ['token'],
  data() {
    return {
      error: '',
      form: {
        name: '',
        phonk: '',
        description: '',
        map: 0,
        trustedRusi: []
      },
      newId: '',
      items: [image0, image1, image2, image3, image4, image5, image6]
    }
  },
  mounted() {
    if (!this.token) {
      this.$router.push('/login')
    }
    const name = this.$route.query.name
    console.log(name)
    if (name) {
      getUdar(this.token, name)
        .then((udar) => {
          this.form.name = udar.name
          this.form.phonk = udar.phonk
          this.form.description = udar.description
          this.form.map = udar.map
        })
        .catch((error) => {
          if (error.message === 'Auth error') {
            this.$emit('onlogin', null)
            this.$router.push('/')
            return
          }
          console.log(error)
          this.error = error.message
        })
    }
  },
  methods: {
    teachNew() {
      if (!this.form.name) {
        this.error = 'Позабыл имечко'
        return
      }
      if (!this.form.phonk) {
        this.error = 'Позабыл фонк праволсавный'
        return
      }
      if (!this.form.description) {
        this.error = 'Позабыл описание приёма великого'
        return
      }
      const name = this.form.name
      teachUdar(this.token, this.form)
        .then(() => this.$router.push({ path: '/learn-udar', query: { name } }))
        .catch((error) => {
          if (error.message === 'Auth error') {
            this.$emit('onlogin', null)
          }
          console.log(error)
          this.error = error.message
        })
    },
    newTrusted() {
      if (!this.newId) {
        return
      }
      this.form.trustedRusi.push(parseInt(this.newId))
      this.newId = ''
    }
  },
  computed: {
    computedStr() {
      return this.form.trustedRusi.join(', ')
    }
  }
}
</script>

<style scoped>
.carousel {
  border-radius: 1px;
  border: 30px solid white;
  border-image: url('@/assets/border.png') 200 round round;
}
.teach input {
  width: 400px;
}
.slavic-button.smaller {
  border: 10px solid white;
  border-image: url('@/assets/border.png') 200 round round;
  border-radius: 100%;
  padding: 0px 10px;
  height: 40px;
}
#newRus {
  width: 100px;
  background: #efefef;
  border-radius: 3px;
  border: 0;
  color: #377d6a;
}
.field.extra {
  flex-direction: column;
  display: flex;
  gap: 10px;
}
.adder {
  display: flex;
  flex-direction: row;
  justify-content: center;
}
.helper {
  position: absolute;
  top: -20px;
}
</style>
