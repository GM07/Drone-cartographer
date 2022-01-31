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
      branches: 80,
      functions: 80,
      lines: 80,
    },
  },
  moduleFileExtensions: ['js', 'ts', 'vue'],
  setupFiles: ['./tests/unit/setutp_tests.ts'],
};
