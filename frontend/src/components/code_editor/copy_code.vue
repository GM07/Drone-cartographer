<template>
  <div
    class="copy_code"
    :style="{width: width, height: height}"
    tabindex="0"
    @click="copy"
    @focusout="resetMessage"
  >
    <div class="tooltip">{{ message }}</div>
    <textarea ref="textarea" readonly tabindex="1" :value="content"></textarea>
    <svg
      class="feather feather-copy"
      fill="none"
      height="100%"
      :stroke="color"
      stroke-linecap="round"
      stroke-linejoin="round"
      stroke-width="2"
      viewBox="0 0 24 24"
      width="100%"
      xmlns="http://www.w3.org/2000/svg"
    >
      <rect height="13" rx="2" ry="2" width="13" x="9" y="9"></rect>
      <path d="M5 15H4a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h9a2 2 0 0 1 2 2v1"></path>
    </svg>
  </div>
</template>

<script>
/* eslint-disable */
export default {
  name: 'CopyCode',
  props: {
    content: {
      type: String,
    },
    width: {
      type: String,
      default: '20px',
    },
    height: {
      type: String,
      default: '20px',
    },
    color: {
      type: String,
      default: '#aaa',
    },
  },
  data() {
    return {
      message: 'Copy code',
    };
  },
  methods: {
    selectContent() {
      if (this.$refs !== null) {
        this.$refs.textarea.select();
      }
      document.execCommand('copy');
    },
    copy(event) {
      this.selectContent();
      event.target.focus();
      this.message = 'Copied!';
    },
    resetMessage() {
      this.message = 'Copy code';
    },
  },
};
</script>

<style scoped>
.copy_code {
  transition: 0.2s opacity ease;
  position: relative;
  opacity: 0.5;
  width: 20px;
  height: 20px;
  cursor: pointer;
}
.copy_code:focus {
  outline: none;
}
.copy_code > textarea {
  user-select: none;
  position: absolute;
  padding: 0;
  width: 0;
  height: 0;
  background: transparent;
  resize: none;
  opacity: 0;
  border-color: rgba(0, 0, 0, 0);
}
.copy_code > svg {
  pointer-events: none;
}
.copy_code:hover {
  opacity: 1;
}
.copy_code:hover > .tooltip {
  display: block;
}
.copy_code > .tooltip {
  font-family: sans-serif;
  display: none;
  position: absolute;
  bottom: -10px;
  left: -96px;
  font-size: 12px;
  color: white;
  width: 84px;
  height: 30px;
  line-height: 30px;
  background: rgba(0, 0, 0, 0.8);
  box-sizing: border-box;
  text-align: center;
  border-radius: 4px;
}
</style>
