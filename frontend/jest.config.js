module.exports = {
  preset: '@vue/cli-plugin-unit-jest/presets/typescript-and-babel',
  collectCoverage: true,
  collectCoverageFrom: [
    'src/**/*.{vue,ts,tsx}',
    '!src/main.ts',
    '!**/node_modules/**',
    '!**/vendor/**',
  ],
  coverageThreshold: {
    global: {
      branches: 0,
      functions: 0,
      lines: 0,
    },
  },
  moduleFileExtensions: ['js', 'ts', 'vue'],
  setupFiles: ['./tests/unit/setutp_tests.ts'],
};
