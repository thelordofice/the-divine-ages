#include "Components/InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
	: MaxInventorySlots(100)
	, CurrentCarryWeight(0.0f)
	, MaxCarryWeight(200.0f)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::AddItem(uint16 ItemID, uint8 Quantity)
{
	FInventoryItem* ExistingItem = FindItem(ItemID);

	if (ExistingItem)
	{
		ExistingItem->Quantity = FMath::Min(255, ExistingItem->Quantity + Quantity);
	}
	else
	{
		if (InventoryItems.Num() >= MaxInventorySlots)
		{
			RPG_WARNING("Inventory is full");
			return;
		}

		FInventoryItem NewItem;
		NewItem.ItemID = ItemID;
		NewItem.Quantity = Quantity;
		NewItem.Level = 1;
		NewItem.Rarity = MedievalRPG::EItemRarity::Common;
		NewItem.bEquipped = false;

		InventoryItems.Add(NewItem);
	}

	CurrentCarryWeight += Quantity;
	BroadcastInventoryChanged();

	RPG_LOG("Item %d added (quantity: %d)", ItemID, Quantity);
}

void UInventoryComponent::RemoveItem(uint16 ItemID, uint8 Quantity)
{
	FInventoryItem* Item = FindItem(ItemID);

	if (!Item)
	{
		RPG_WARNING("Item %d not found in inventory", ItemID);
		return;
	}

	if (Item->Quantity <= Quantity)
	{
		CurrentCarryWeight -= Item->Quantity;
		InventoryItems.RemoveSingle(*Item);
	}
	else
	{
		Item->Quantity -= Quantity;
		CurrentCarryWeight -= Quantity;
	}

	BroadcastInventoryChanged();
	RPG_LOG("Item %d removed (quantity: %d)", ItemID, Quantity);
}

void UInventoryComponent::EquipItem(uint16 ItemID)
{
	FInventoryItem* Item = FindItem(ItemID);

	if (Item)
	{
		Item->bEquipped = true;
		BroadcastInventoryChanged();
		RPG_LOG("Item %d equipped", ItemID);
	}
}

void UInventoryComponent::UnequipItem(uint16 ItemID)
{
	FInventoryItem* Item = FindItem(ItemID);

	if (Item)
	{
		Item->bEquipped = false;
		BroadcastInventoryChanged();
		RPG_LOG("Item %d unequipped", ItemID);
	}
}

void UInventoryComponent::DropItem(uint16 ItemID, uint8 Quantity)
{
	RemoveItem(ItemID, Quantity);
	RPG_LOG("Item %d dropped", ItemID);
}

bool UInventoryComponent::HasItem(uint16 ItemID, uint8 Quantity) const
{
	const FInventoryItem* Item = FindItem(ItemID);
	return Item && Item->Quantity >= Quantity;
}

void UInventoryComponent::CombineItems(uint16 ItemID1, uint16 ItemID2, uint16 ResultID)
{
	if (!HasItem(ItemID1) || !HasItem(ItemID2))
	{
		RPG_WARNING("Missing required items for combination");
		return;
	}

	RemoveItem(ItemID1);
	RemoveItem(ItemID2);
	AddItem(ResultID);

	RPG_LOG("Items combined: %d + %d = %d", ItemID1, ItemID2, ResultID);
}

int32 UInventoryComponent::GetItemCount(uint16 ItemID) const
{
	const FInventoryItem* Item = FindItem(ItemID);
	return Item ? Item->Quantity : 0;
}

FString UInventoryComponent::GetItemName(uint16 ItemID) const
{
	return FString::Printf(TEXT("Item_%d"), ItemID);
}

void UInventoryComponent::BroadcastInventoryChanged()
{
	OnInventoryChanged.Broadcast(InventoryItems);
}

FInventoryItem* UInventoryComponent::FindItem(uint16 ItemID)
{
	for (FInventoryItem& Item : InventoryItems)
	{
		if (Item.ItemID == ItemID)
		{
			return &Item;
		}
	}
	return nullptr;
}

const FInventoryItem* UInventoryComponent::FindItem(uint16 ItemID) const
{
	for (const FInventoryItem& Item : InventoryItems)
	{
		if (Item.ItemID == ItemID)
		{
			return &Item;
		}
	}
	return nullptr;
}
