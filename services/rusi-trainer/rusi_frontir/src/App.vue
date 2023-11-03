<template>
  <div class="bg-color"></div>
  <audio ref="myaudio" style="display: none">
    <source src="@/assets/PhonkTechnoSigmaByEmerelGrayInfraction.mp3#t=00:00:11" type="audio/mpeg">
  </audio>
  <header>
    <nav>
      <router-link class="main-link" to="/">
        <h1>RUSI TRAINER</h1>
      </router-link>
    </nav>
    <router-link to="/drinker">
      <img class="drink-button" src="@/assets/drink.png" alt="drink-water" />
    </router-link>
  </header>
  <router-view @onlogin="onlogin" @playphonk="playphonk" @stopphonk="stopphonk" :token="token"> </router-view>
</template>
<script>
export default {
  name: 'App',
  data() {
    return {
      token: localStorage.getItem('token')
    }
  },
  methods: {
    onlogin(token) {
      this.token = token
      localStorage.setItem('token', token)
    },
    playphonk(){
      this.$refs.myaudio.volume = 0.4;
      this.$refs.myaudio.play();
    },
    stopphonk(){
      this.$refs.myaudio.pause();
    }
  }
}
</script>

<style>
.groster {
  font-size: 22px;
  margin-top: -40px;
  text-align: center;
}
.error {
  color: darkred;
}
.card {
  background-color: rgba(255, 255, 255, 0.6);
  border: 30px solid white;
  border-image: url('@/assets/border.png') 200 round round;
  display: flex;
  flex-direction: column;
  gap: 50px;
  padding: 60px 50px;
}
.link {
  color: #013d05;
}
.card .field {
  position: relative;
}
.container {
  display: flex;
  align-items: center;
  justify-content: center;
  width: 100%;
  height: 100%;
  min-height: calc(100vh - 250px);
}

.slavic-button.slavic-button__lg {
  padding: 10px 40px;
  font-size: 40px;
  border: 30px solid white;
  border-image: url('@/assets/border.png') 200 round round;
}
.slavic-button {
  padding: 5px 20px;
  font-size: 25px;

  background-color: rgba(150, 150, 150, 0.5);
  text-shadow:
    1px 1px 2px green,
    0 0 1em white,
    0 0 0.2em blue;
  color: black;
  opacity: 0.9;
  border-radius: 5px;
  border: 12px solid white;
  border-image: url('@/assets/border.png') 200 round round;
}
.slavic-button:hover {
  background-color: rgba(150, 150, 150, 0.9);
  text-shadow:
    1px 1px 2px blue,
    0 0 1em white,
    0 0 0.2em green;
  opacity: 1;
}
.bg-color {
  position: fixed;
  left: 0;
  top: 0;
  right: 0;
  z-index: -1;
  filter: blur(3px);
  background-image: url('assets/animka2.gif');
  background-position: center;
  background-size: cover;
  min-height: 100vh;
  width: 100vw;
}

body {
  background-color: #665034;
}
@font-face {
  font-family: 'Tsarevich';
  src: url('assets/Tsarevich old.ttf');
  font-weight: normal;
  font-style: normal;
}
body {
  font-family: 'Tsarevich';
}
input {
  font-family: 'Tsarevich';
  font-size: 1.5em;
}
button {
  font-family: 'Tsarevich';
}
.gate {
  width: 300px;
  padding: 10px 0 10px 15px;

  font-weight: 400;
  color: #377d6a;
  background: #efefef;
  border: 0;
  border-radius: 3px;
  outline: 0;
  text-indent: 65px;
  transition: all 0.3s ease-in-out;
}
.gate::-webkit-input-placeholder {
  color: #efefef;
  text-indent: 0;
  font-weight: 300;
}
.gate + label {
  display: inline-block;
  position: absolute;
  top: 0;
  left: 0;
  padding: 8px 10px;
  text-shadow: 0 1px 0 rgba(19, 74, 70, 0.4);
  background: #7ab893;
  transition: all 0.4s ease-in-out;
  border-top-left-radius: 3px;
  border-bottom-left-radius: 3px;
  transform-origin: left bottom;
  z-index: 99;
  font-size: 1.3em;
}
.gate + label:before,
.gate + label:after {
  content: '';
  position: absolute;
  top: 0;
  right: 0;
  bottom: 0;
  left: 0;
  border-radius: 3px;
  background: #377d6a;
  transform-origin: left bottom;
  transition: all 0.4s ease-in-out;
  pointer-events: none;
  z-index: -1;
}
.gate + label:before {
  background: rgba(3, 36, 41, 0.2);
  z-index: -2;
  right: 20%;
}

span:nth-child(2) .gate {
  text-indent: 85px;
}

span:nth-child(2) .gate:focus,
span:nth-child(2) .gate:active {
  text-indent: 0;
}

.gate:focus,
.gate:active {
  color: #377d6a;
  text-indent: 0;
  background: #fff;
  border-top-right-radius: 3px;
  border-bottom-right-radius: 3px;
}
.gate:focus::-webkit-input-placeholder,
.gate:active::-webkit-input-placeholder {
  color: #aaa;
}
.gate:focus + label,
.gate:active + label {
  transform: rotate(-66deg);
  border-radius: 3px;
}
.gate:focus + label:before,
.gate:active + label:before {
  transform: rotate(10deg);
}
</style>
<style scoped>
header {
  width: 100%;
  display: flex;
  text-align: center;
  justify-content: center;
  background: transparent;
}
.main-link {
  font-size: 50px;
  color: black;
  text-shadow:
    1px 1px 2px red,
    0 0 1em white,
    0 0 0.2em green;
  -webkit-animation: glowing-text 5s ease-in-out infinite alternate;
  -moz-animation: glowing-text 5s ease-in-out infinite alternate;
  animation: glowing-text 5s ease-in-out infinite alternate;
}
@-webkit-keyframes glowing-text {
  0% {
    text-shadow:
      0 0 10px #fff,
      0 0 20px #fff,
      0 0 30px #2e00e6,
      0 0 40px #6300e6,
      0 0 50px green,
      0 0 60px green,
      0 0 70px green;
  }

  50% {
    text-shadow:
      1px 1px 2px red,
      0 0 1em white,
      0 0 0.2em green;
  }

  100% {
    text-shadow:
      0 0 20px #fff,
      0 0 30px #ff4da6,
      0 0 40px #2e00e6,
      0 0 50px #ff4da6,
      0 0 60px #6300e6,
      0 0 70px red;
  }
}
.drink-button {
  position: absolute;
  width: 50px;
  height: 50px;
  right: 0;
  cursor: pointer;
  filter: invert(0.1s);
  box-shadow: darkred 1px 1px 20px;
}
</style>
