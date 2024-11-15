import React, { Component } from 'react'
import { Text, View , StyleSheet} from 'react-native'

export class Timer extends Component {
  render() {
    return (
        <View style={styles.screen}>
        <Text style={styles.text}>This is the TIMER screen</Text>
      </View>
    )
  }
}


const styles = StyleSheet.create({
    screen: {
      flex: 1,
      justifyContent: 'center',
      alignItems: 'center',
    },
    text: {
      fontSize: 18,
    },
  });
  


export default Timer
