/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef _FORWARD_ERROR_CORRECTION_H_
#define _FORWARD_ERROR_CORRECTION_H_

#include <vector>
#include <list>
#include "Decoder.h"


using std::list;
using std::vector;

class ForwardErrorCorrection 
{
 public:
  // Maximum number of media packets we can protect
  static const unsigned int kMaxMediaPackets = 48u;

  ForwardErrorCorrection();
  virtual ~ForwardErrorCorrection();

  HPR_INT32 GenerateFEC(const PacketList& media_packet_list,
                      HPR_UINT8 protection_factor, int num_important_packets,
                      HPR_BOOL use_unequal_protection, FecMaskType fec_mask_type,
                      PacketList* fec_packet_list);

  HPR_INT32 DecodeFEC(ReceivedPacketList* received_packet_list,
                    RecoveredPacketList* recovered_packet_list);

  int GetNumberOfFecPackets(int num_media_packets, int protection_factor);


  static int PacketOverhead();


  void ResetState(RecoveredPacketList* recovered_packet_list);

 private:
  typedef list<FecPacket*> FecPacketList;

  void QuickSortInsert(RecoveredPacketList* recovered_packet_list,RecoveredPacket* recoverd_packet_to_insert);

  void GenerateFecUlpHeaders(const PacketList& media_packet_list,
                             HPR_UINT8* packet_mask, HPR_BOOL l_bit,
                             int num_fec_packets);

  int InsertZerosInBitMasks(const PacketList& media_packets,
                            HPR_UINT8* packet_mask, int num_mask_bytes,
                            int num_fec_packets);

  static void InsertZeroColumns(int num_zeros, HPR_UINT8* new_mask,
                                int new_mask_bytes, int num_fec_packets,
                                int new_bit_index);

  static void CopyColumn(HPR_UINT8* new_mask, int new_mask_bytes,
                         HPR_UINT8* old_mask, int old_mask_bytes,
                         int num_fec_packets, int new_bit_index,
                         int old_bit_index);

  void GenerateFecBitStrings(const PacketList& media_packet_list,
                             HPR_UINT8* packet_mask, int num_fec_packets,
                             HPR_BOOL l_bit);

  void InsertPackets(ReceivedPacketList* received_packet_list,RecoveredPacketList* recovered_packet_list);

  int InsertMediaPacket(ReceivedPacket* rx_packet,
                         RecoveredPacketList* recovered_packet_list);

  void UpdateCoveringFECPackets(RecoveredPacket* packet);


  int InsertFECPacket(ReceivedPacket* rx_packet, const RecoveredPacketList* recovered_packet_list);

  static void AssignRecoveredPackets(FecPacket* fec_packet, const RecoveredPacketList* recovered_packets);

  //void InsertRecoveredPacket(RecoveredPacket* rec_packet_to_insert,
  //                           RecoveredPacketList* recovered_packet_list);

  void AttemptRecover(RecoveredPacketList* recovered_packet_list);

  static void InitRecovery(const FecPacket* fec_packet,
                           RecoveredPacket* recovered);

  static void XorPackets(const Packet* src_packet, RecoveredPacket* dst_packet);

  static void FinishRecovery(RecoveredPacket* recovered);

  void RecoverPacket(const FecPacket* fec_packet,
                     RecoveredPacket* rec_packet_to_insert);

  static int NumCoveredPacketsMissing(const FecPacket* fec_packet);

  static void DiscardFECPacket(FecPacket* fec_packet);

  static void DiscardOldPackets(RecoveredPacketList* recovered_packet_list);
	
  void PrintfFec();

  void PrintfMedia();
  static HPR_UINT16 ParseSequenceNumber(HPR_UINT8* packet);

  vector<Packet> generated_fec_packets_;
  FecPacketList fec_packet_list_;
  HPR_BOOL fec_packet_received_;
};

#endif 

