<template>
  <div class="block">
    <h1 class="helper" title="GO AWAY LIZARD!!!">УХОДИ ПОДЛЫЙ ЯЩЕР!!!</h1>
  </div>
</template>

<script>
export default {
  name: 'BlockView'
}
</script>

<style scoped>
.block {
  position: fixed;
  width: 100%;
  height: 100%;
  z-index: 99999;
  background-image: url('@/assets/lock.jpg');
  background-position: center;
  top: 0;
  display: flex;
  justify-content: center;
  align-items: center;
}
.helper {
  color: white;
  text-shadow:
    black 4px 4px 3px,
    red 4px 4px 3px,
    gold 4px 4px 3px;
  font-size: 10vw;
}
</style>
