module.exports = {
  preset: '@vue/cli-plugin-unit-jest/presets/typescript-and-babel',
  verbose: true,
  collectCoverage: true,
  collectCoverageFrom: [
    'src/**/*.{vue,ts,tsx}',
    '!src/main.ts',
    '!src/app.vue',
    '!src/components/code_editor/**',
    '!**/node_modules/**',
    '!**/vendor/**',
  ],
  coverageReporters: ['clover', 'json', 'lcov', 'text', 'html'],
  coverageThreshold: {
    global: {
      branches: 0,
      functions: 0,
      lines: 0,
    },
  },
  moduleNameMapper: {
    '@/(.*)$': '<rootDir>/src/$1',
  },
  roots: ['<rootDir/src/', '<rootDir>/tests/'],
  moduleFileExtensions: ['js', 'ts', 'vue'],
  transformIgnorePatterns: ['/node_modules/(?!@riophae/vue-treeselect)/'], // Fix for https://githubhot.com/repo/vuejs/vue-jest/issues/257
  transform: {
    '^.+\\.js$': 'babel-jest',
    '^.+\\.vue$': 'vue-jest',
  },
  snapshotSerializers: ['<rootDir>/node_modules/jest-serializer-vue'],
  setupFiles: ['./tests/unit/setutp_tests.ts'],
};
